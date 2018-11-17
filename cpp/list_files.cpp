// Vsevolod Ivanov

#include <string>
#include <iostream>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

int main()
{
    std::string dirpath = ".";
    directory_iterator itr(dirpath);
    directory_iterator end_itr;

    for (; itr != end_itr; ++itr)
    {
        if (!is_directory(itr->status()))
        {
            std::cout << itr->path().filename() << std::endl;
            std::cout << itr->path().stem().string() << std::endl;
        }
    }
}
