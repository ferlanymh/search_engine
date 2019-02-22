#include "searcher.h"
#include "../common/util.hpp"
namespace searcher
{
  //查正派索引函数实现，返回n号html文件的信息结构体doc_info
  const DocInfo* index::GetDocInfo(uint64_t doc_id) const
  {
    if (doc_id >= Forward_index.size())//说明下标越界
    {
      std::cout<<"doc_id is illegal"<<std::endl;
      return nullptr;
    }
    return &Forward_index[doc_id];
  }
  
  //查倒排索引函数实现，返回包含词word的html编号和词频构成的结构体
  const InvertedList* index::GetInvertedList(const std::string word) const
  {
    //直接在unorder_map表示的倒排索引中找word就行了 
    auto pos = Inverted_index.find(word);//在unordered_map中查找应该返回pair的迭代器

    if (pos == Inverted_index.end())//没找到
    {
      return nullptr;
    }

    return &pos->second;//我们需要的是html编号和词频构成的结构体，刚好是迭代器的second
    
  }
  bool Bulid(const std::string& input_path)
  {
    std::cout<<"build index start!"<<std::endl;
    //1.先打开我们处理好的html行文本文件，对其中每一行进行解读
    std::ifstream file(input_path.c_str());
    if (!file.is_open())
    {
      std::cout<<"file can't open"<<std::endl;
      return false;
    }
    
    std::string line;
    while (std::getline(file,line))//开始对每一行进行解读
    {
      //2.构建Docinfo对象，插入到正派索引中
     const DocInfo* doc_info = BuildForward(line);
      
      //3.构建倒排索引
      BuildInverted(doc_info);
    }
    std::cout<<"build index finished!"<<std::endl;
    file.close();
    return true;
  }
  const DocInfo* index::BuildForward(const std::string& line)
  {
    //1.对这一行内容进行切分
    //   切分字符串放到了工具类util.hpp中
    std::vector<std::string> tokens; //放置切分后的数据
    StringUtil::Split(line,&tokens,"\3");
    if (tokens.size() != 3)
    {
      std::cout<<"tokens is not right!"<<std::endl;
      return nullptr;
    }

    //2.构造一个Docinfo结构体
    DocInfo doc_info;
    //3.把这个对象插入到正派索引中
    doc_info.title = tokens[0];
    doc_info.url = tokens[1];
    doc_info.content = tokens[2];
    doc_info.doc_id = Forward_index.size();

    Forward_index.push_back(doc_info);
    return &Forward_index[Forward_index.size() - 1];
  }
  void index::BuildInverted(const DocInfo& doc_info)
  {
    //1.先对得到的doc_info进行分词(标题和正文都要分)
    std::vector<std::string> title_tokens;//放正文分词结果
    CutWord(doc_info.title,&title_tokens);
    std::vector<std::string> content_tokens;//放正文分词结果 
    CutWord(doc_info.content,&title_tokens);

    //2.对doc_info中的标题和正文进行词频统计
    //    这里要说明标题中出现的权重应该大于正文
    //
    struct WordCount//定义一个结构体记录标题和正文中的词频
    {
      int title_count;
      int content_count;
    };
    
    std::unordered_map<std::string,WordCount> StringAndWordCount;//用hash表完成词频统计
    for (std::string& word : title_tokens)//遍历标题分词结果,统计词频
    {
      //补充:在统计词频是应该忽略大小写,    比如我查hello,HELLO应该算出现2次
      boost::to_lower(word);//全倒成小写
      ++(StringAndWordCount[word].title_count);
    }

    for (std::string& word : content_tokens)//遍历正文分词结果,统计词频
    {
      boost::to_lower(word);
      ++(StringAndWordCount[word].content_count);
    }

    //3.遍历分词结果,在倒排索引查找
    for (const auto& iter: StringAndWordCount)
    {
      //4.如果倒排索引中没有这个分词,则在倒排索引中构建新的键值对,键值对中存储
      //<词，存放word_count的数组>
      //5.如果倒排索引中已经有这个分词,则找到对应的值vector，构建一个新的Weight
      //对象并插入。
      

      //这里有点难理解，我们之前统计了各个词在这个html中标题和正文中出现的频数，
      //但是我们存到倒排索引中时只用记一个总频数就可以了.
      //而且别忘了，倒排索引中<key,value>中的value是vector<Weight>,
      //而Weight又由doc_id和weight组成。所以我们我们的最终目的就是weight。
      //构建他则要算出总频数。
      
      Weight w;
      w.doc_id = doc_info.doc_id;
      w.weight = 10 * ((iter.second).title_count) + 
              (iter.second).content_count;
      //typedef std::vector<Weight> InvertedList; 
      //这一步已经包括了第四和第五点，因为unordered_map的operator[]具有查找功能，存在返回引用，不存在则创建后返回引用。我们直接对其vector<Weight>进行插入w就可以了。
      InvertedList& invertedList = Inverted_index[iter.first];
      
      invertedList.push_back(w);
      return ;
    }
    
  }

 
}
