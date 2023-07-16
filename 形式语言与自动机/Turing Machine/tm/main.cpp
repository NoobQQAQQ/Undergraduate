#include"tm.h"

using namespace std;

int main(int argc, char *argv[])
{
	TM tm(argv[1]);
	tm.parse();
	tm.execute();
}
