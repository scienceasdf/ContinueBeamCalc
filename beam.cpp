#include"beam.h"
#include<fstream>
#include<algorithm>
#include"chasemethod.h"
#include<iostream>

void beam::loadjizhongli(double pos, double force)
{
    if(force!=0){
        loads[pos]+=force;
    }
}

void beam::loadMoment(double pos, double me)
{
    if(me!=0){
        meLoads[pos]+=me;
    }
}

void beam::loadlin(double& pos1,double& pos2,double& force1,double& force2)
{
    for(int i=0;i<201;i++)
    {
        loads[pos1+i*(pos2-pos1)/200.0]+=(force1+i*(force2-force1)/200.0)*(pos2-pos1)/200;
    }
}

double beam::thetaA()
{
    double dx=1e-8;
    double dv=bending(dx);
    return dv/dx;

}

double beam::thetaB()
{
    double dx=1e-8;
    double dv=-bending(leng-dx);
    //std::cout<<dx<<"\t\t\t"<<dv<<"!!!\n";
    return dv/dx;
}


//----------------------------------------------------------------------------------------------------

double simpSupBeam::bending(const double& pos)
{
    #define left iter->first
    #define force iter->second
    #define right (leng-iter->first)

    double v=0;
    std::map<double,double>::iterator p=loads.end();
    for(std::map<double,double>::iterator iter=loads.begin() ;iter!=p;++iter)
    {
           v+=(pos<left)?-force*right*pos*(leng*leng-pos*pos-right*right)/6.0:
               -force*right*(leng/right*(pos-left)*(pos-left)*(pos-left)+(leng*leng-right*right)*pos-pos*pos*pos)/6.0;
    }
    #undef force

    p=meLoads.end();
    #define me (iter->second)

    for(std::map<double,double>::iterator iter=meLoads.begin();iter!=p;++iter)
    {
        v+=(pos<left)?me*pos*(leng*leng-3.0*right*right-pos*pos)/6.0:
            me*(-pos*pos*pos+3*leng*(pos-left)*(pos-left)+(leng*leng-3.0*right*right)*pos)/6.0;
    }

    return v/Ela/intia/leng;

    #undef left
    #undef me
    #undef right
}

double simpSupBeam::wanju(const double& pos)
{
    double f=0;
    std::map<double,double>::iterator p=loads.end();
    for(std::map<double,double>::iterator iter=loads.begin() ;iter!=p;++iter)
    {
        f+=(pos<iter->first)?pos*(leng-iter->first)*iter->second/leng:
            (leng-pos)*(iter->first)*iter->second/leng;
    }
    p=meLoads.end();
    for(std::map<double,double>::iterator iter=meLoads.begin() ;iter!=p;++iter)
    {
        f+=(pos<iter->first)?-pos*iter->second/leng:-pos*iter->second/leng+iter->second;
    }

    return f;
}

double simpSupBeam::neili(const double& pos)
{
    double f=0;
    std::map<double,double>::iterator p=loads.end();
    for(std::map<double,double>::iterator iter=loads.begin() ;iter!=p;++iter)
    {
        f+=(pos<iter->first)?-(leng-iter->first)*iter->second/leng:
            (iter->first)*iter->second/leng;
    }
    p=meLoads.end();
    for(std::map<double,double>::iterator iter=meLoads.begin() ;iter!=p;++iter){
        f+=iter->second/leng;
    }
    return f;
}



//----------------------------------------------------------------------------------------

double overHangingBeam::wanju(const double& pos)
{
    double f=0;
    std::map<double,double>::iterator p=loads.end();
    for(std::map<double,double>::iterator iter=loads.begin() ;iter!=p;++iter)
    {
        if((iter->first)<rRestraint){
            f+=(pos<rRestraint)?((pos<iter->first)?pos*(rRestraint-iter->first)*iter->second/rRestraint:
                (rRestraint-pos)*(iter->first)*iter->second/rRestraint) : 0;}
        else{
            f-=(pos<iter->first)?((pos<rRestraint)?pos*(iter->first-rRestraint)*iter->second/rRestraint:
                                                   iter->second*(leng-pos)-iter->second*(leng-iter->first)) : 0;
        }


    }
    p=meLoads.end();
    for(std::map<double,double>::iterator iter=meLoads.begin() ;iter!=p;++iter)
    {
        f+=(pos<iter->first)?-pos*iter->second/leng:-pos*iter->second/leng+iter->second;
    }

    return f;
}


double overHangingBeam::neili(const double &pos){
    double f=0;
    std::map<double,double>::iterator p=loads.end();
    for(std::map<double,double>::iterator iter=loads.begin() ;iter!=p;++iter)
    {
        if(iter->first<rRestraint){
            f+=(pos<rRestraint)?((pos<iter->first)?-(rRestraint-iter->first)*iter->second/rRestraint:
                (iter->first)*iter->second/rRestraint):0;}
        else
        {
            f+=(pos<iter->first)?((pos<rRestraint)?(iter->second*iter->first/rRestraint-iter->second):
                                                   -iter->second):0;
        }
    }
    p=meLoads.end();
    for(std::map<double,double>::iterator iter=meLoads.begin() ;iter!=p;++iter)
    {
        if(iter->first<rRestraint){
            f+=(pos<rRestraint)?iter->second/leng:0;
        }
        else{
            f+=(pos<rRestraint)?iter->second/leng:0;
        }
    }
    return f;
}

double overHangingBeam::bending(const double & pos){
#define force iter->second
#define ll rRestraint
#define aa iter->first
#define bb (ll-aa)
//#define loadPos iter->first
    double v=0;
    std::map<double,double>::iterator p=loads.end();
    for(std::map<double,double>::iterator iter=loads.begin() ;iter!=p;++iter)
    {
        if(iter->first<rRestraint){
            v+=(pos<ll)?((pos<aa)?-force*bb*pos/6.0/ll*(ll*ll-pos*pos-bb*bb):
                                           -force*bb/6.0/ll*(ll/bb*(pos-aa)*(pos-aa)*(pos-aa)+(ll*ll-bb*bb)*pos-pos*pos*pos)):
                                 (pos-ll)*(force*aa*bb*(ll+aa)/6.0/ll);
        }
#undef aa
#define aa (iter->first-ll)
        else{
            v+=(pos<iter->first)?((pos<ll)?force*aa*pos*(ll*ll-pos*pos)/6.0/ll:
                                           -force*(pos-ll)/6.0*(aa*(3.0*pos-ll)-(pos-ll)*(pos-ll))):
                                force*(-(pos-iter->first)*(aa/6.0)*(2.0*ll+3.0*aa)-aa*aa/3.0*(iter->first));
        }
    }
#undef force
#undef aa
#define me iter->second

    p=meLoads.end();
#define aa iter->first
    for(std::map<double,double>::iterator iter=meLoads.begin();iter!=p;++iter)
    {
        if(iter->first>rRestraint){
            v+=(pos<iter->first)?((pos<rRestraint)?me*(-pos*pos*pos/rRestraint+rRestraint*pos)/6.0:
                                                   -me*(pos*pos*.5-rRestraint*pos/3.0-rRestraint*rRestraint/6.0)):
                                    me*((-iter->first*iter->first*.5+rRestraint*iter->first/3.0+rRestraint*rRestraint/6.0)+
                                        (2.0*rRestraint/3.0-iter->first)*(pos-iter->first));
        }
        else{
            v+=(pos<rRestraint)?((pos<iter->first)?me*pos*(ll*ll-3.0*bb*bb-pos*pos)/ll:
                                                   me/ll*(-pos*pos*pos+3.0*ll*(pos-aa)*(pos-aa)+(ll*ll-3.0*bb*bb)*pos)):
                                                   (pos-ll)*me/6.0*(ll*ll-3*aa*aa);
        }
#undef me
#undef ll
#undef aa
#undef bb
    }
    return v/Ela/intia;
}

/*double overHangingBeam::thetaB()
{
    double dx=1e-5;
    double dv=-bending(rRestraint-dx);
    return dv/dx;
    std::cout<<dx<<"\t\t\t"<<dv<<"!!!\n";
}
*/
//-------------------------------------------------------------------------------

void continBeam::addRes(double pos)
{
    int n=resPos.size();
    //std::cout<<n;
    if(n>2){
        delete beams[n-2];
        beams[n-2]=new simpSupBeam(1,Ela,intia);
    }
    resPos.push_back(pos);
    std::sort(resPos.begin(),resPos.end());
    //double* p=new double[n+1];
    beams.push_back(new simpSupBeam(1,Ela,intia));
    n++;
    //std::cout<<n;
    for(int i=0;i<n-1;i++){
        beams[i]->leng=resPos[i+1]-resPos[i];
    }
    beams[n-1]->leng=leng-resPos[n-1];
    delete beams[n-2];
    beams[n-2]=new simpSupBeam(leng-resPos[n-2],Ela,intia);
}

void continBeam::loadjizhongli(double pos, double force)
{
    if(force!=0){
        jizhongPos.insert(pos);
        loads[pos]+=force;
    }
}

void continBeam::loadMoment(double pos, double me)
{
    if(me!=0){
        jizhongPos.insert(pos);
        meLoads[pos]+=me;
    }
}

void continBeam::loadlin(double pos1,double pos2,double force1,double force2)
{
    for(int i=0;i<201;i++)
    {
        loads[pos1+i*(pos2-pos1)/200.0]+=(force1+i*(force2-force1)/200.0)*(pos2-pos1)/200;
    }
}

void continBeam::calc()
{
    int i=0;
    double pos=resPos[1];
    double f;
    int max=beams.size();
    for(int k=0;k<max;k++){
        beams[k]->loads.clear();
        beams[k]->meLoads.clear();
    }
    std::map<double,double>::iterator p=loads.end();
    for(std::map<double,double>::iterator iter=loads.begin();iter!=p;++iter){
        while(iter->first>pos){
            ++i;
            pos+=beams[i]->leng;
        }
        f=(iter->second);
        beams[i]->loadjizhongli(iter->first-pos+beams[i]->leng,f);
    }
    p=meLoads.end();
    i=0;
    for(std::map<double,double>::iterator iter=meLoads.begin();iter!=p;++iter){
        while(iter->first>pos){
            ++i;
            pos+=beams[i]->leng;

        }
        beams[i]->loadMoment(iter->first-pos+beams[i]->leng,iter->second);
    }
    int n=resPos.size()-2;
    double *a=new double[n-1], *b=new double[n], *c=new double[n-1],
            *d=new double[n], *m;
    pos=Ela*intia;

    for(int i=0;i<n-1;++i){
        a[i]=beams[i+1]->leng/6.0/pos;
        b[i]=(beams[i]->leng+beams[i+1]->leng)/3.0/pos;
        c[i]=a[i];
        d[i]=beams[i+1]->thetaA()-beams[i]->thetaB();

    }





    b[n-1]=(beams[n-1]->leng+beams[n]->leng)/3.0/pos;

    d[n-1]=beams[n]->thetaA()-beams[n-1]->thetaB();

    m=root(a,b,c,d,n);

    delete []a; delete []b; delete []c; delete []d;

    /*!!!!!!!!!!!!!!!!*/
    beams[0]->loadMoment(beams[0]->leng,-m[0]);//

    beams[n]->loadMoment(0,m[n-1]);
    /**/



    for(int k=1;k<=n-1;++k){
        beams[k]->loadMoment(beams[k]->leng,-m[k]);
        beams[k]->loadMoment(0,m[k-1]);

    }

    delete [] m;
    double x;
    pos=0;
    i=0;
    double relaPos;
    for(int k=0; k<101;++k){
        x=k*leng/100.0;
        while(x>=pos && x!=leng){
            ++i;
            pos+=beams[i-1]->leng;
        }

        relaPos=x-pos+beams[i-1]->leng;
        bend[k]=beams[i-1]->bending(relaPos);
        Fsy[k]=beams[i-1]->neili(relaPos);
        innerMz[k]=beams[i-1]->wanju(relaPos);
    }
}

double continBeam::bending(const double &pos)
{
    int i=0;
    double relaPos;
    double x=0;

    while(x<pos){
         ++i;
         x+=beams[i-1]->leng;
    }
    relaPos=pos-x+beams[i-1]->leng;
    return beams[i-1]->bending(relaPos);
}

double continBeam::wanju(const double &pos)
{
    int i=0;
    double relaPos;
    double x=0;

    while(x<pos){
         ++i;
         x+=beams[i-1]->leng;
    }
    relaPos=pos-x+beams[i-1]->leng;
    return beams[i-1]->wanju(relaPos);
}

double continBeam::neili(const double &pos)
{
    int i=0;
    double relaPos;
    double x=0;

    while(x<pos){
         ++i;
         x+=beams[i-1]->leng;
    }
    relaPos=pos-x+beams[i-1]->leng;
    return beams[i-1]->neili(relaPos);
}
