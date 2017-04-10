# 2017HW_Championshi
这是2017 “华为杯” 软件精英挑战赛

我们编写的程序入口在 deploy.cpp 中
包含的头文件和源文件有：
Init.h		Init.cpp
Topology.h	Topology.cpp
plantgrowth.h	plantgrowth.cpp
geneticAL.h 	geneticAL.cpp
cluster.h	cluster.cpp

Init.h		是初始化文件。这个文件将所给的数据转化为图的数据结构。
geneticAL.h	使用了遗传算法求出网络节点中的关键节点。
Topology.h	使用的是最小费用最大流算法,求出所给点形成的路径。
cluster.h 	是分簇算法。根据相连性将所有的网络节点分为不同的簇。
plantgrowth.h	是模拟植物生长算法。

解题思路：
初始化后，将所有的点分簇，在分簇中使用遗传算法求出关键节点。再整合所有的节点，最后使用模拟植物生长算法，求出全局最优解。
