#include <iostream>
#include <functional>
#include "QueueTester.h"
#include "SetTester.h"
#include "TimeTester.h"

using namespace std;

int main(int argc, char* argv[]) {
    size_t threadsNum = 1;
    size_t testSize = 1000;
    if (argc != 3) {
        cout << "input 2 arguments: number of threads and size of test per thread\n";
        cout << "continue work with 1 thread and 1000 numbers per thread\n";
    }
    else {
        threadsNum = atoi(argv[1]);
        testSize = atoi(argv[2]);
    }
    cout << "--- Basic test section ---\n";
    QueueCommonTester qct(testSize);
    cout << "As SCSP queue is implemented, only common queue test is implemented with 1 reading and 1 parallely writing threads.\n";
    cout << "SCSP queue test result: " << qct.execute() << endl;
    SetReadersTester srt(threadsNum, testSize);
    cout << "Result of lazy set readers test with " << threadsNum << " thread(s): " << srt.execute() << endl;
    SetWritersTester swt(threadsNum, testSize);
    cout << "Result of lazy set writers test with " << threadsNum << " thread(s): " << swt.execute() << endl;
    SetCommonTester sct(threadsNum, testSize);
    cout << "Result of lazy set common test with " << threadsNum << " thread(s): " << sct.execute() << endl;
    cout << "--- Time test section ---\n";
    cout << "As SCSP queue is implemented, common time test was not implemented because of uselessness.\n";
    ReadersTimeTester rttr(testSize, threadsNum, true);
    cout << "Readers time test with random data generation:\n";
    cout << rttr.execute() << endl;
    ReadersTimeTester rtts(testSize, threadsNum, false);
    cout << "Readers time test with sequential data generation:\n";
    cout << rtts.execute() << endl;
    WritersTimeTester wttr(testSize, threadsNum, true);
    cout << "Writers time test with random data generation:\n";
    cout << wttr.execute() << endl;
    WritersTimeTester wtts(testSize, threadsNum, false);
    cout << "Writers time test with sequential data generation:\n";
    cout << wtts.execute() << endl;
    return 0;
}
