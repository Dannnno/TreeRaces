// Stupid mingw port of gtest 
#ifdef MINGW_COMPILER
	#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
	#endif
#endif

#include "gtest/gtest.h"

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
  	return RUN_ALL_TESTS();
}
