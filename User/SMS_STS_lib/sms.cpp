#include <string>
#include <regex>
#include "sms.hpp"

int SMT::smt_add(int a, int inc){
    return a + inc;
};
SMT::SMT(){

}

extern "C"{ 
    void* SMT_create(){
        return new SMT();
    }
    int _c_smt_add(void* smt_obj, int a, int inc){
        int result = static_cast<SMT*>(smt_obj)->smt_add(a, inc);
        return result;
    }
}
