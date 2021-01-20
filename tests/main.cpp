#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

class __declspec(dllexport) TestRunner {
public:
	void run(int, char**);
	static boost::unit_test::test_suite* init(int, char**);
};

void TestRunner::run(int n, char** argc) {
	boost::unit_test::unit_test_main(&TestRunner::init, n, argc);
}
boost::unit_test::test_suite* TestRunner::init(int, char**) {
	return 0;
}


 #include <boost/test/test_exec_monitor.hpp>

 int test_main(int argv, char** const argc)
 {
	 TestRunner runner;
	 runner.run(argv, argc);
	 return 0;
 }