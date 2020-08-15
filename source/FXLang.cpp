#include "fxlang/FXLang.h"

namespace fxlang
{

CU_SINGLETON_DEFINITION(FXLang);

extern void regist_rttr();

FXLang::FXLang()
{
	regist_rttr();
}

}