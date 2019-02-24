# search_engine

# 项目内容
实现一个基于Boost文档库(Boost Version: 1.69.0)的小型搜索引擎，

功能：用户输入关键字，搜索引擎能快速检索出包含关键字的Boost文档的标题、URL及摘要。

## 开发环境
C++/Linux g++5.3.1

## 项目流程

![Image text](https://github.com/ferlanymh/search_engine/blob/master/%E6%90%9C%E7%B4%A2%E5%BC%95%E6%93%8E%E9%A1%B9%E7%9B%AE%E6%B5%81%E7%A8%8B%E5%9B%BE.png)
## 模块划分
本项目主要分成三个模块：数据解析模块，索引模块，搜索模块。

### 数据解析模块：
  此模块主要负责过滤出Boost文档库中所有的html文件路径，并将其对应文件中的标题，URL，正文依次保存在结构体中，供后面的索引模块使用。
  
### 索引模块：
  此模块根据数据解析模块的输出结果，构建正排索引和倒排索引。
  正排索引能通过给定html编号，找出对应html文件的标题，URL，正文。
  倒排索引能通过给定关键字，找出该关键字在哪些html文件中出现过及出现的词频
  
  本项目中正排索引采用vector存储实现，倒排索引采用unordered_map存储实现。

### 搜索模块
  此模块围绕索引模块展开，能够通过用户键入关键字，查找出对应的html文档。
  
  主要分为分词/查找/排序/构建返回结果4部分。
  
  1. 分词：将用户输入的关键字进行分词处理，本项目中采用的是jieba分词    资料地址：https://github.com/yanyiwu/cppjieba
  2. 查找：将每个分词结果在倒排索引中查找，得到各个词在哪些html文件中出现及词频。
  3. 排序：将所有的查找结果根据出现的词频进行排序，词频高的排在前面。
  4. 构建：根据排序结果，依次查找正排索引，得到对应html文件的标题，URL，正文。

本项目引入了外部的httplib头文件，使用户能够通过http请求的方式对Boost文档进行查询。资料地址：https://github.com/yhirose/cpp-httplib
  
### 效果图  
  目前的效果图只能将查找的内容陈列出来，作者并不懂很多前端知识，但也正在尝试修改中。
#### 查询关键字class
![Image text](https://github.com/ferlanymh/search_engine/blob/master/%E6%9F%A5%E8%AF%A2%E5%85%B3%E9%94%AE%E5%AD%97class%E7%9A%84%E6%95%88%E6%9E%9C%E5%9B%BE.png)


#### 查询关键字Home Libraries

![Image text](https://github.com/ferlanymh/search_engine/blob/master/%E6%9F%A5%E8%AF%A2%E5%85%B3%E9%94%AE%E5%AD%97Home%20Libraries.png)
