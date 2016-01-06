#include "String.h"

int main()
{
  String s1 = "My";
  std::cout << s1 << std::endl;

  String s2 = " String";
  s1 += s2;
  std::cout << s1 << std::endl;
  
  String s3 = s1 + " overloaded";
  for(size_t i = 0; i != s3.size(); ++i)
    std::cout << s3[i] << " ";
  std::cout << std::endl;

  return 0;
}
