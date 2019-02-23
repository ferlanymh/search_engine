#pragma once

//构建索引模块和搜索模块
//要构建正排索引和倒排索引
#include<string>
#include<vector>
#include<unordered_map>
#include<iostream>
#include<fstream>
#include<jsoncpp/json/json.h>
#include"../cppjieba/include/cppjieba/Jieba.hpp"

namespace searcher
{
  struct DocInfo//在正排索引中使用，用于存储各个html文件中的各类信息
  {
    int doc_id;
    std::string title;
    std::string url;
    std::string content;
  };

  struct Weight//倒排索引中的value,表示某个词在某个文档中出现过，以及词的权重是多少 
  {
    uint64_t doc_id;//根据内容获得id号
    int weight;//词频权重，为后面的排序做准备(因为词频高的应该放在前面)
    std::string key;//表示出现的词是啥
  };

  typedef std::vector<Weight> InvertedList;

  class index//索引模块，包含倒排索引和正排索引
  {
    private:
      //用vector构建正派索引，直到编号获取内容，编号在这儿就是下标
      std::vector<DocInfo> Forward_index;

      //用unordered_map构建倒排索引，根据内容获得编号和词频，使用底层是hash表的unorder_map为佳。
      std::unordered_map<std::string,InvertedList> Inverted_index;
      
      cppjieba::Jieba jieba;//使用外部的分词方式jieba分词

    public:
      //build中的子函数，构建正排
      const DocInfo* BuildForward(const std::string& line);

      //build中的子函数，构建倒排;
      void BuildInverted(const DocInfo& doc_info);
      //构建正排索引和倒排索引！最重要的函数
      bool Bulid(const std::string& input_path);
      

      //查找正排索引，返回n号html文件的各项值
      const DocInfo* GetDocInfo(uint64_t doc_id) const ;
      

      //查找倒排索引，返回这个词在哪些编号的html文件中出现过以及出现的词频
      const InvertedList* GetInvertedList(const std::string word) const ;
      
      //其实就是调用一下cppjiaba中的CutForWord方法
      void CutWord(const std::string& input,std::vector<std::string>* output);

      index();//构造函数
  
  };

class Searcher//搜索模块
{
    //1.分词：对要搜索的字段进行分词
    //2.查找：对所有的分词结果依次在倒排索引中查找
    //3.排序：根据查找的结果中词频的大小进行排序，毕竟出现次数多的更应该放前面
    //4.构造返回结果：根据查找结果中的编号，查找正排索引，输出搜索的最终内容
private:
  index* index_;
public:
  Searcher():index_(new index())
  {}

  ~Searcher()
  {
    delete index_;
  }

  bool init(const std::string& input_path);//加载索引
  bool search(const std::string &query,std::string* result);//搜索函数，query输入参数，result输出参数


private: 
  std::string MakeDesc(const std::string& content,std::string key);  

};

} //end
