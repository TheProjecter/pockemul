#include "common.h"
#include "Log.h"
#include "cpu.h"
#include "Debug.h"

CCPU::CCPU(CPObject *parent)
{				//[constructor]
    pPC = (CpcXXXX*) parent;

    halt=0;				//halt?(0:none, 1:halting)
    end=0;				//program end?(0:none, 1:end)
    savesw=1;			//end with memory save?(0:no, 1:yes)
    log=0;				//execute log?(0:off, 1:on)
    logsw=false;			//log mode?(0:off, 1:on)
    resetFlag = false;
    usestatus=0;
    fp_status=0;
    fp_log=0;
    fn_log="cpu.log";
    CallSubLevel=prevCallSubLevel=0;

    for (int i=0;i<0x200;i++) imem[i]=0;

}

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
    if( logsw && (!fp_log) ) fp_log=fopen(fn_log,"wt");	// Open log file
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
