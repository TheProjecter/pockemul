#include "common.h"
#include "Log.h"
#include "cpu.h"

void CCPU::save(void)
{
	AddLog(LOG_MASTER,tr("Save status..."));
	QFile file(fn_status);
	
	if (file.open(QIODevice::WriteOnly)) {
		save_internal(&file);
		file.close();
		AddLog(LOG_MASTER,"done.");
	}
	else AddLog(0x01,tr("write error!"));
}
 
void CCPU::Check_Log(void)
{
	if( logsw && (!fp_log) ) fp_log=fopen("lh5801.log","wt");	// Open log file
	if( (!logsw) && fp_log ) fclose(fp_log);					// Close log file
}

bool CCPU::exit(void)
{
	if(logsw) fclose(fp_log);							//close log file

	pDEBUG->exit();

	if(usestatus)
	{
		save();
	}
	return true;
}
