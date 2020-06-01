#include <string>

namespace stringtools
{
  // Get first token of string
		inline std::string firstToken(const std::string &in)
		{
			if (!in.empty())
			{
				size_t token_start = in.find_first_not_of(" \t");
				size_t token_end = in.find_first_of(" \t", token_start);
				if (token_start != std::string::npos && token_end != std::string::npos)
				{
					return in.substr(token_start, token_end - token_start);
				}
				else if (token_start != std::string::npos)
				{
					return in.substr(token_start);
				}
			}
			return "";
		}
	
	// Change file name in string containing a path
	inline std::string changeFileNameInPath(std::string path, std::string newFileName){
		size_t endPath = path.find_last_of("/") + 1;
		return (path.substr(0, endPath) + newFileName);
	}
}