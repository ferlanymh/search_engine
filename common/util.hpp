#pragma once 
#include<string>
#include<fstream>
#include<vector>
#include<boost/algorithm/string.hpp>
class FileUtil//文件读写工具类
{
public:
  static bool Read(const std::string& file_path,std::string* str)
  {
    //目的是将file_path路径的内容读取到str中，方便我们对str进行操作。
    std::ifstream file(file_path.c_str());//定义ifstream对文件进行读取
    
    if (!file.is_open())//判断是否打开成功
      return false;

    //采用一行一行的读取方式
    std::string line;
    while (std::getline(file,line))//当还有行可以读取时继续
    {
      *str += line;
      *str += "\n";//保留\n
    }

    file.close();
    return true;
  
  }
  
  
  static bool Write(const std::string& file_path,const std::string& str)//这里两个参数都是const，因为我们不需要修改
  {
    //目的是将str内的内容写入到file_path所对应的路径内
    std::ofstream file(file_path.c_str());
    if (!file.is_open())
      return false;

    //直接write就可以了
    file.write(str.c_str(),str.size());//读取起点是str，读取长度就是str的长度，非常简单
    file.close();
    return true;
  } 
};

class StringUtil
{
  public:
  //基于boost的字符串切分函数
  static void Split(const std::string& input,std::vector<std::string>* output,
                    const std::string& split_char)
  {
    boost::split(*output,input,boost::is_any_of(split_char),//支持多分隔符切分，只要是split_char内的字符都切分
                boost::token_compress_off);//关闭合并   aaa\3\3bbb   能切分出3个子串。
  }
};

