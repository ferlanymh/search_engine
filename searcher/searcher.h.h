#pragma once

//构建索引模块和搜索模块
//要构建正排索引和倒排索引
#include<string>
#include<vector>
#include<unordered_map>
#include<iostream>
#include<fstream>
namespace searcher
{
  struct DocInfo//在正排索引中使用，用于存储各个html文件中的各类信息
  {
    int doc_id;
    std::string title;
    std::string url;
    std::string content;
  };

  struct Weight//倒排索引中的value 
  {
    uint64_t doc_id;//根据内容获得id号
    int weight;//词频权重，为后面的排序做准备(因为词频高的应该放在前面)
  };

  typedef std::vector<Weight> InvertedList;

  struct index//索引模块，包含倒排索引和正排索引
  {
    private:
      //用vector构建正派索引，直到编号获取内容，编号在这儿就是下标
      std::vector<DocInfo> Forward_index;

      //用unordered_map构建倒排索引，根据内容获得编号和词频，使用底层是hash表的unorder_map为佳。
      std::unordered_map<std::string,InvertedList> Inverted_index;
    
    public:
      //构建正排索引和倒排索引！最重要的函数
      bool Bulid(const std::string& input_path);
      

      //查找正排索引，返回n号html文件的各项值
      const DocInfo* GetDocInfo(uint64_t doc_id) const ;
      

      //查找倒排索引，返回这个词在哪些编号的html文件中出现过以及出现的词频
      const InvertedList* GetInvertedList(const std::string word) const ;
    
    private:
      //build中的子函数，构建正排
      const DocInfo* BuildForward(const std::string& line);

      //build中的子函数，构建倒排;
      void BuildInverted(const DocInfo& doc_info);
  };

  class Searcher//搜索模块
  {
    
  };

} //end
