#include <vector>
#include <iostream>


struct Evaluator
{
    std::vector<double> d;
    std::vector<char*> s;
    std::vector<int> opData;
    
    typedef int(*OpF)(Evaluator* e,int pc,int opDataStart,int opDataEnd);

    struct Op
    {
        OpF f;
        int opDataStart;
        int opDataEnd;
    };
    std::vector<Op> ops;

    void eval(){
        int pc=0;
        while(pc<ops.size()){
            const Op& op=ops[pc];
            pc++;
            pc=op.f(this,pc,op.opDataStart,op.opDataEnd);
        }
    }
};

int add(Evaluator* eval,int pc,int opDataStart,int opDataEnd)
{
    eval->d[eval->opData[opDataStart]]=eval->d[eval->opData[opDataStart+1]]+eval->d[eval->opData[opDataStart+2]];
    return pc;
}

int promote(Evaluator* eval,int pc,int opDataStart,int opDataEnd)
{
    int dimOut=eval->opData[opDataStart];
    int posIn=eval->opData[opDataStart+1];
    int posOut=eval->opData[opDataStart+2];
    for(int k=posOut;k<posOut+dimOut;k++) eval->d[k]=eval->d[posIn];
}


int main()
{
    Evaluator eval;
    Evaluator::Op op;
    eval.d.push_back(5);
    eval.d.push_back(1);
    eval.d.push_back(7);
    // promote result
    eval.d.push_back(0);
    eval.d.push_back(0);
    // add result
    eval.d.push_back(0);
    eval.d.push_back(0);

    // setup promote
    op.f=promote;
    op.opDataStart=eval.opData.size();
    eval.opData.push_back(2); // dim
    eval.opData.push_back(2); // posIn
    eval.opData.push_back(3); // posOut
    op.opDataEnd=eval.opData.size();
    eval.ops.push_back(op);

    // setup add
    op.f=add;
    op.opDataStart=eval.opData.size();
    eval.opData.push_back(2);
    eval.opData.push_back(0);
    eval.opData.push_back(3);
    eval.opData.push_back(5);
    op.opDataEnd=eval.opData.size();
    eval.ops.push_back(op);


    eval.eval();
    for(int i=0;i<eval.d.size();i++) std::cerr<<"d["<<i<<"]="<<eval.d[i]<<std::endl;

    
    
}



