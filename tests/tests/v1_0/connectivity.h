#ifndef TESTS_CONNECTIVITY_H
#define TESTS_CONNECTIVITY_H

class Connectivity: public ::testing::Test{
protected:
    void TearDown() override {
        IPC::endTests();
        VirtualNetwork::resetNetwork();
    }
};

TEST_F(Connectivity, t1){
    topology2();


}

#endif //TESTS_CONNECTIVITY_H
