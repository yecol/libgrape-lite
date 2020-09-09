/** Copyright 2020 Alibaba Group Holding Limited.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef GRAPE_COMMUNICATION_COMMUNICATOR_H_
#define GRAPE_COMMUNICATION_COMMUNICATOR_H_

#include <mpi.h>

#include <algorithm>
#include <memory>

#include "grape/communication/sync_comm.h"
#include "grape/serialization/in_archive.h"
#include "grape/serialization/out_archive.h"

namespace grape {

/**
 * @brief Communicator provides methods to implement distributed aggregation,
 * such as Min/Max/Sum.
 *
 */
class Communicator {
 public:
  Communicator() : comm_(NULL_COMM) {}
  ~Communicator() {
    if (ValidComm(comm_)) {
      MPI_Comm_free(&comm_);
    }
  }
  void InitCommunicator(MPI_Comm comm) { MPI_Comm_dup(comm, &comm_); }

  template <typename T>
  void SendTo(fid_t fid, const T& msg) {
    int dst_worker = fid;
    InArchive arc;
    arc << msg;
    SendArchive(arc, dst_worker, comm_);
  }

  template <typename T>
  void RecvFrom(fid_t fid, T& msg) {
    int src_worker = fid;
    OutArchive arc;
    RecvArchive(arc, src_worker, comm_);
    arc >> msg;
  }

  template <typename T, typename FUNC_T>
  void AllReduce(const T& msg_in, T& msg_out, const FUNC_T& func) {
    int worker_id, worker_num;
    MPI_Comm_rank(comm_, &worker_id);
    MPI_Comm_size(comm_, &worker_num);
    if (worker_id == 0) {
      msg_out = msg_in;
      for (int src_worker = 1; src_worker < worker_num; ++src_worker) {
        T got_msg;
        RecvFrom<T>(src_worker, got_msg);
        func(msg_out, got_msg);
      }
      for (int dst_worker = 1; dst_worker < worker_num; ++dst_worker) {
        SendTo<T>(dst_worker, msg_out);
      }
    } else {
      SendTo<T>(0, msg_in);
      RecvFrom<T>(0, msg_out);
    }
  }

  template <typename T>
  void Max(const T& msg_in, T& msg_out) {
    AllReduce<T>(msg_in, msg_out,
                 [](T& lhs, const T& rhs) { lhs = std::max(lhs, rhs); });
  }

  template <typename T>
  void Min(const T& msg_in, T& msg_out) {
    AllReduce<T>(msg_in, msg_out,
                 [](T& lhs, const T& rhs) { lhs = std::min(lhs, rhs); });
  }

  template <typename T>
  void Sum(const T& msg_in, T& msg_out) {
    AllReduce<T>(msg_in, msg_out, [](T& lhs, const T& rhs) { lhs += rhs; });
  }

  void ArchiveAllGather(const InArchive& in_archive, OutArchive& out_archive) {
    int worker_id, worker_num;
    MPI_Comm_rank(comm_, &worker_id);
    MPI_Comm_size(comm_, &worker_num);

    size_t send_byte_count = in_archive.GetSize();
    size_t* total_byte_lens =
        reinterpret_cast<size_t*>(malloc(sizeof(size_t) * worker_num));
    MPI_Allgather(&send_byte_count, sizeof(size_t), MPI_CHAR, total_byte_lens,
                  sizeof(size_t), MPI_CHAR, comm_);

    size_t total_byte_count = 0;
    for (int i = 0; i < worker_num; ++i) {
      total_byte_count += total_byte_lens[i];
    }

    out_archive.Allocate(total_byte_count);
    if (worker_id == 0) {
      char* ptr = out_archive.GetBuffer();
      for (int src_worker = 1; src_worker < worker_num; ++src_worker) {
        recv_buffer<char>(ptr, total_byte_lens[src_worker], src_worker, comm_,
                          src_worker);
        ptr += total_byte_lens[src_worker];
      }
      memcpy(ptr, in_archive.GetBuffer(), send_byte_count);
      for (int dst_worker = 1; dst_worker < worker_num; ++dst_worker) {
        send_buffer<char>(out_archive.GetBuffer(), total_byte_count, dst_worker,
                          comm_, 0);
      }
    } else {
      send_buffer<char>(in_archive.GetBuffer(), send_byte_count, 0, comm_,
                        worker_id);
      recv_buffer<char>(out_archive.GetBuffer(), total_byte_count, 0, comm_, 0);
    }

    free(total_byte_lens);
  }

 private:
  MPI_Comm comm_;
};

template <typename APP_T>
typename std::enable_if<std::is_base_of<Communicator, APP_T>::value>::type
InitCommunicator(std::shared_ptr<APP_T> app, MPI_Comm comm) {
  app->InitCommunicator(comm);
}

template <typename APP_T>
typename std::enable_if<!std::is_base_of<Communicator, APP_T>::value>::type
InitCommunicator(std::shared_ptr<APP_T> app, MPI_Comm comm) {}

}  // namespace grape

#endif  // GRAPE_COMMUNICATION_COMMUNICATOR_H_
