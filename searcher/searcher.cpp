#include "searcher.h"
#include "../common/util.hpp"
namespace searcher
{
  const char* const DICT_PATH = "./jieba_dict/jieba.dict.utf8";
  const char* const HMM_PATH = "./jieba_dict/hmm_model.utf8";
  const char* const USER_DICT_PATH = "./jieba_dict/user.dict.utf8";
  const char* const IDF_PATH = "./jieba_dict/idf.utf8";
  const char* const STOP_WORD_PATH = "./jieba_dict/stop_words.utf8";
  
  //教程说要先用词典文件初始化jiaba对象
  index::index() : jieba(DICT_PATH,HMM_PATH,USER_DICT_PATH,IDF_PATH,STOP_WORD_PATH)
  {}

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
  


  bool index::Bulid(const std::string& input_path)
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
      BuildInverted(*doc_info);

      if (doc_info->doc_id % 500 == 0)
        std::cout<<"already build: "<<(doc_info->doc_id)<<std::endl;
    }
  /*  
  for (auto it : (index::Inverted_index))
  {
    std::cout<<"doc_id= "<<it.first<<std::endl;
  }
  */

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
    CutWord(doc_info.content,&content_tokens);

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
      w.key = iter.first;//把出现的词记录一下，方便后面使用
      
      //typedef std::vector<Weight> InvertedList; 
      //这一步已经包括了第四和第五点，因为unordered_map的operator[]具有查找功能，存在返回引用，不存在则创建后返回引用。我们直接对其vector<Weight>进行插入w就可以了。
      InvertedList& invertedList = Inverted_index[iter.first];
      
      invertedList.push_back(w);
    }
  }
    

    void index::CutWord(const std::string& input,std::vector<std::string>* output)
    {
      //直接使用外部构造的结巴分词方法
      jieba.CutForSearch(input,*output);

    }


    //////////////////////////////////////////////////////////////////////
    //下面代码是搜索模块的实现
    //
    
    //初始化Searcher
    bool Searcher::init(const std::string& input_path)
    {
      return index_->Bulid(input_path);
    }

    bool Searcher::search(const std::string& query,std::string* json_result)
    {
      //1.分词：对要搜索的字段进行分词                              
      std::vector<std::string> tokens;
      index_->CutWord(query,&tokens);
      

      std::vector<Weight> all_tokens_result;//弄一个Weight的数组暂时存储每个分词对应的查找结果

      //2.查找：对所有的分词结果依次在倒排索引中查找,找出 出现分词的html文件编号和词频
      for (std::string word : tokens)
      {
        auto* ret = index_->GetInvertedList(word);
        if (ret == nullptr)
        {
          continue;//这里查找不到说明在倒排索引中没有这个分词，跳过即可。
        }

        //将当前分词结果暂存all_tokens_result
        all_tokens_result.insert(all_tokens_result.end(),ret->begin(),ret->end());
      }
      
      //3.排序：根据查找的结果中词频的大小进行排序，毕竟出现次数多的更应该放前面      
      std::sort(all_tokens_result.begin(),all_tokens_result.end(),
          [](const Weight& w1,const Weight& w2){//这里使用一下匿名函数，作为自定义的比较函数
              return w1.weight > w2.weight;
          });
      
      //4.构造返回结果：根据查找结果中的编号，查找正排索引，输出搜索的最终内容
      //
      //为了组织最终的输出内容，我用了简单的JSON格式
      /*预期结果：
      [
        {
          "title": "这时标题",
          "desc": "这时描述",
          "url": "这时url",
        },

        {                                                                                     
          "title": "这时标题",                                                                   
          "desc": "这时描述",
          "url": "这时url",                                                           
        }, 

          ...

      ] 
      */
      Json::Value Results;//存所有最终的搜索结果
      for (const auto& weight : all_tokens_result)
      {
        auto* doc = index_->GetDocInfo(weight.doc_id);//找正排，拿到doc_info结构体
        if (doc == nullptr)//没找到
          continue;

      //现在已经拿到了一条搜索的最终结果，使用线程的json第三方库jsoncpp
        
        Json::Value result;//存当前这一条搜索结果
        result["desc"] = MakeDesc(doc->content,weight.key) ;//正文太长了，构建一个摘要比较合适
        result["url"] = doc->url;
        result["title"] = doc->title;
        Results.append(result);//插入到最终结果中
        
      }
    
      Json::StyledWriter writer;
      *json_result = writer.write(Results);
      return true;
    }
   
    std::string Searcher::MakeDesc(const std::string& content,std::string key)//构建摘要
    {
      size_t pos = content.find(key);
      if (pos == std::string::npos)//正文中没有key，而在标题中
      {
        if (content.size() < 100)
          return content;
        else
          return content.substr(0,100) + "...";
      }

      //说明找到了,那么向前取一部分，向后取一部分，构成摘要
      size_t begin = pos < 50 ? 0 : pos - 50;//向前取50个字节，不足则从0下标开始
      if (pos + 50 >= content.size())//向后取50个字节，不足则全取
        return content.substr(begin);
      else
        return content.substr(begin,100)+ "..." ;

    }

}//end namespace searcher 
