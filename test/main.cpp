#define CATCH_CONFIG_RUNNER
#include <catch/catch.hpp>

#include <fxlang/FXLang.h>

int main(int argc, char* argv[])
{
    fxlang::FXLang::Instance();

    int result = Catch::Session().run(argc, argv);    

    return result;
}