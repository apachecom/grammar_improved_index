//
// Created by alejandro on 27-11-19.
//

#include "CLogger.h"

const string CLogger::m_sFileName = "Log.txt";
CLogger *CLogger::m_pThis = NULL;
ofstream CLogger::m_Logfile;
CLogger::Model CLogger::model = CLogger::Model();


CLogger::CLogger()
{

}

CLogger* CLogger::GetLogger()
{
    if (m_pThis == NULL){
        m_pThis = new CLogger();
        m_Logfile.open(m_sFileName.c_str(), ios::out | ios::app);
    }
    return m_pThis;
}
//
//
//void CLogger::write_stats()
//{
//    m_Logfile << "\n\nCOLL,"+model.coll+"\n";
//    m_Logfile << "Sigma,"+std::to_string(model.sigma)+"\n";
//    m_Logfile << "Iterations,"+std::to_string(model.num_it) + "\n";
//    m_Logfile << "Repair\n";
//    m_Logfile << "Compress Time(msec)\n";
//    for(auto && it:model.repair_times_in_each_it)
//        m_Logfile << std::to_string(it) << ",";
//    m_Logfile <<"\n";
//
//    m_Logfile << "Number of Rules\n";
//    for(auto && it:model.repair_rules_in_each_it)
//        m_Logfile << std::to_string(it) << ",";
//    m_Logfile <<"\n";
//
//    m_Logfile << "Size of first rule\n";
//    for(auto && it:model.repair_seq_in_each_it)
//        m_Logfile << std::to_string(it) << ",";
//    m_Logfile <<"\n";
//
//    m_Logfile << "Parser:\n";
//    m_Logfile << "Building time(msec):\n";
//    for(auto && it:model.parser_times_in_each_it)
//        m_Logfile << std::to_string(it) << ",";
//    m_Logfile <<"\n";
//
//    m_Logfile << "Dictionary size:\n";
//    for(auto && it:model.total_size_dicc_in_each_it)
//        m_Logfile << std::to_string(it) << ",";
//    m_Logfile <<"\n";
//
//    m_Logfile << "Dictionary num of phrases:\n";
//    for(auto && it:model.len_dicc_in_each_it)
//        m_Logfile << std::to_string(it) << ",";
//    m_Logfile <<"\n";
//
//    m_Logfile << "Seq size:\n";
//    for(auto && it:model.total_size_parse_in_each_it)
//        m_Logfile << std::to_string(it) << ",";
//    m_Logfile <<"\n";
//
//    m_Logfile << "Seq len:\n";
//    for(auto && it:model.len_parser_in_each_it)
//        m_Logfile << std::to_string(it) << ",";
//    m_Logfile <<"\n";
//    m_Logfile << "Posprocessing Time(msec),"+std::to_string(model.posprocessing_time)+"\n";
//    m_Logfile <<"\n";
//
//}

void CLogger::Log(const string& sMessage)
{
    //m_Logfile <<  Util::CurrentDateTime() << ":\t";
    m_Logfile << sMessage << "\n";
}

CLogger& CLogger::operator<<(const string& sMessage)
{
    //m_Logfile << "\n" << Util::CurrentDateTime() << ":\t";
    m_Logfile << sMessage << "\n";
    return *this;
}