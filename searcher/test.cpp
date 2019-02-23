#include "searcher.h"
#if 0
int main()
{
  searcher::index index_;
  bool ret = index_.Bulid("../data/tmp/output");
  if (!ret)
  {
    std::cout<<"Bulid failed"<<std::endl;
    return 1;
  
  }

  auto* inverted_list = index_.GetInvertedList("filesystem");
  if (inverted_list == nullptr) 
  {
    std::cout<<"GetInvertedList failed"<<std::endl;
    return 1;
    
  }
  for (auto weight : *inverted_list)
  {
    std::cout<<"id = "<<weight.doc_id<<std::endl;
    const auto* doc_ = index_.GetDocInfo(weight.doc_id);

    std::cout<<"title = "<<doc_->title<<std::endl;
    std::cout<<"url = "<<doc_->url<<std::endl;
  }
  return 0;
}
#endif

#if 1
int main()
{
  searcher::Searcher s;
  bool ret = s.init("../data/tmp/output");
  if (!ret)
    return 1;

  std::string query = "filesystem";
  std::string result;
  s.search(query,&result);
  std::cout<<"result:"<<std::endl;
  std::cout<<result<<std::endl;
  return 0;
}
#endif
