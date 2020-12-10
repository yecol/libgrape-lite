# Performance

We evaluated performance of **libgrape-lite** with [LDBC Graph Analytics Benchmark](http://graphalytics.org/). In addition to the ease of programming, we find that **libgrape-lite** achieves high performance comparably to the state-of-the-art systems. The experiments were conducted on 4 instances of [r6.8xlarge](https://www.alibabacloud.com/help/doc-detail/25378.htm#d12e563) on [AlibabaCloud ECS](https://www.alibabacloud.com/product/ecs), each with 32 threads, over LDBC XL-size datasets. Instances are imaged with [Aliyun Linux (a CentOS-variant)](https://www.alibabacloud.com/help/doc-detail/111881.htm).

We compared **libgrape-lite** with [PowerGraph](https://github.com/jegonzal/PowerGraph)(commit a038f97
) [GeminiGraph](https://github.com/thu-pacman/GeminiGraph)(commit 170e7d3
) and [Plato](https://github.com/Tencent/plato)(commit 21009d6). Each system is built with GCC(v4.8.5) and MPICH(v3.1). To make the comparisons fair, **libgrape_lite** was built with HUGE_PAGES and jemalloc disabled.

We made minor changes on their code and datasets:
- Turned on `-O3` optimization for all three systems.
- Added timing stubs for Plato.
- Replaced the random-pick logic with the deterministic logic from LDBC for the CDLP in Plato.
- Changed the weight type of SSSP from `float` to `double` in GeminiGraph.
- Changed the load strategy from `load_directed` to `load_undirected_from_directed` for PageRank in GeminiGraph
- Reformat the datasets to adapt Plato and GeminiGraph's formats (e.g., 0-based continuous vertex ids)

## Results
The results are reported below. The numbers in the table represent the evaluation time in seconds. 
The best results are marked in **bold**.

| Algorithm |     Dataset    |      PowerGraph     |     GeminiGraph     |      Plato      |     libgrape-lite     |
|-----------|----------------|---------------------|---------------------|-----------------|-----------------------|
|    SSSP   | datagen-9_0-fb |        6.48         |        0.74         |  N/A  |        **0.52**           |
|    WCC    | datagen-9_0-fb |        16.99        |        1.05         |       2.85      |        **0.52**           |
|    BFS    | datagen-9_0-fb |        4.31         |        0.38         |      0.73       |        **0.23**           |
|  PageRank | datagen-9_0-fb |        26.90        |  X    |      X   |    **1.61**    |
|    CDLP   | datagen-9_0-fb |        1535.09      |  N/A    |     16.67   |    **8.49**    |
|    LCC    | datagen-9_0-fb |        453.07       |      N/A            |      N/A      |         **16.53**       |


We used “default” code provided by the competitor systems when it is available. 
- **N/A** indicate that the system didn't provide the application. And
- **X**  indicates the results produced are not consistent with the verified results provided by LDBC.

The inconsistences of PageRank come from different settings on convergence conditions. 
To give a comprehensive comparison, we made our best efforts to revise our application([pagerank_local.h](examples/analytical_apps/pagerank/pagerank_local.h)), making them output the same results as competitor systems.
The performance results are shown as below. 

| Algorithm |     Dataset    |     GeminiGraph     |        Plato        | libgrape-lite   |
|-----------|----------------|---------------------|---------------------|---------------------|
|  PageRank | datagen-9_0-fb |        2.60         |        4.81         |      **1.69**       |
|           | datagen-9_1-fb |        3.24         |        5.17         |      **1.88**       |
|           | datagen-9_2-zf |        7.18         |        41.43        |      **2.99**       |
|           | graph500-26    |        6.61         |        14.67        |      **2.88**       |
|           | com-friendster |        9.90         |        18.65        |      **5.95**       |


## Reproducing the results

We will release a public image containing the script, together with all the systems and datasets on AlibabaCloud and AWS soon.
