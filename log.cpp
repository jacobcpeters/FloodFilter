#include "main.h"


Log::Log(const char* filename) {
  m_stream.open(filename, ios_base::app);
}

void Log::Write(char* logline){
  m_stream << logline << " ";
}

Log::~Log(){
  m_stream.close();
}
