#include "RetiradorDeInformacoes.h"


std::string RetiradorDeInformacoes::debugFilePath;
FILE* RetiradorDeInformacoes::debugFile;
std::map<std::string, std::vector<double> > RetiradorDeInformacoes::statsMap; 
std::vector<std::string > RetiradorDeInformacoes::cuOrderMap; 
bool RetiradorDeInformacoes::encodeStarted;
int RetiradorDeInformacoes::numCodedPics;
double RetiradorDeInformacoes::SAD;
double RetiradorDeInformacoes::MSE;
double RetiradorDeInformacoes::SSE;
double RetiradorDeInformacoes::nonZeroCoeff;
double RetiradorDeInformacoes::cost_2Nx2N;
double RetiradorDeInformacoes::cost_MSM;
double RetiradorDeInformacoes::delta_QP;
double RetiradorDeInformacoes::cu_time;



 

double RetiradorDeInformacoes::getFeatureValue(TComDataCU *&cu, int feat_idx){
    double feat_val = 0;
    
    int cmode;
    int cux = cu->getCUPelX();
    int cuy = cu->getCUPelY();
  
    

    
    switch(feat_idx){
        case 0: feat_val = cu->getPic()->getPOC(); break;
        case 1: feat_val = cux; break;
        case 2: feat_val = cuy; break;
        case 3: feat_val =  width; break;
        case 4: feat_val =  cu->getDepth(0); break;

    }
    return feat_val;
}

void RetiradorDeInformacoes::EVAL_CU(TComDataCU *&cu){
    std::string sstr_str = RetiradorDeInformacoes::getMapString(cu, cu->getDepth(0), 0);

    cuOrderMap.push_back(sstr_str);
    int feat_idx;
    for(feat_idx = 0; feat_idx < N_FEATURES-1; feat_idx++){
        statsMap[sstr_str].push_back(getFeatureValue(cu, feat_idx));
    }
    
  
}


std::string RetiradorDeInformacoes::getMapString(TComDataCU *cu, int depth, int partIdx){
    std::stringstream sstr;
    
    int cu_x = cu->getCUPelX() + g_auiRasterToPelX[ g_auiZscanToRaster[partIdx] ];
    int cu_y = cu->getCUPelY() + g_auiRasterToPelY[ g_auiZscanToRaster[partIdx] ];
    int width = 64 >> depth;

    //int cu_x = pcCU->getCUPelX() ;
    //int cu_y = pcCU->getCUPelY() ;
    sstr << cu->getPic()->getPOC() << "_" << cu_x << "x" << cu_y << "_" << width;
    
    return sstr.str();
}


void RetiradorDeInformacoes::writeStats(){
    //std::map<std::string, std::vector<double> >::iterator it_i;
    //std::vector<double>::iterator it_j;
    
    int i,j;
    for(i = 0; i < cuOrderMap.size(); i++){
        const std::string &s = cuOrderMap[i];
        if (statsMap[s].size() < N_FEATURES) // skipping CUs below the ones that were not split
            continue;
        for(j = 0; j < statsMap[s].size()-1; j++ )
            fprintf(debugFile,"%f,",(double) statsMap[s][j]);
        fprintf(debugFile,"%f\n",(double) statsMap[s][j]);
    }
}

void RetiradorDeInformacoes::printHeader(){

        int i;

    for(i = 0; i < sizeof(featureMap)/sizeof(featureMap[0])-1; i++)
        fprintf(debugFile,"%s,",featureMap[i].c_str());
    fprintf(debugFile,"%s\n",featureMap[i].c_str());
}