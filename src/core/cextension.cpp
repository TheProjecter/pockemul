#include "cextension.h"

CExtension::CExtension(void)
{
    //[Basic constructor] {"",false,false}

    Id = "";
    Description = "";
    IsAvailable = false;
    IsChecked = false;
    needreboot = true;
    Action = 0;
}
CExtension::CExtension(QString Identifier,QString newDescription = "",QString name="",int width=0,int height=0,int pc_X=0,int pc_Y=0,bool Available=false,bool Checked=false,bool reboot=true)
{
    //[Basic constructor] {"",false,false}

    Id = Identifier;
    Description = newDescription;
    fname = name;
    DX = width;
    DY = height;
    offset_pcX = pc_X;
    offset_pcY = pc_Y;

    IsAvailable = Available;
    IsChecked = Checked;
    needreboot = reboot;
    Action = 0;
}


CExtensionArray::CExtensionArray(QString value1,QString value2)							//[constructor]
{
    Id 			= value1;
    Description = value2;
    Menu		= 0;
    emptyAction	= 0;
    actionGroup	= 0;

    ExtArray[ID_CE151]	= new CExtension("ce-151",		"4 Kb memory module");
    ExtArray[ID_CE153]	= new CExtension("ce-153",		"Graphic pad");
    ExtArray[ID_CE155]	= new CExtension("ce-155",		"8 Kb memory module");
    ExtArray[ID_CE156]	= new CExtension("ce-156",		"Kanji characters tape");
    ExtArray[ID_CE157]	= new CExtension("ce-157",		"Kanji characters support module");
    ExtArray[ID_CE158]	= new CExtension("ce-158",		"Centronic and RS-232 interface");
    ExtArray[ID_CE159]	= new CExtension("ce-159",		" 8 Kb lithium battery backup");
    ExtArray[ID_CE160]	= new CExtension("ce-160",		"16 Kb lithium battery backup");
    ExtArray[ID_CE161]	= new CExtension("ce-161",		"16 Kb lithium battery backup");
    ExtArray[ID_CE201M]	= new CExtension("ce-201m",		" 8 Kb Memory card");
    ExtArray[ID_CE202M]	= new CExtension("ce-202m",		"16 Kb Memory card");
    ExtArray[ID_CE203M]	= new CExtension("ce-203m",		"32 Kb Memory card");
    ExtArray[ID_CE210M]	= new CExtension("ce-210m",		" 2 Kb Memory card");
    ExtArray[ID_CE211M]	= new CExtension("ce-211m",		" 4 Kb Memory card");
    ExtArray[ID_CE212M]	= new CExtension("ce-212m",		" 8 Kb Memory card");
    ExtArray[ID_CE2H16M]= new CExtension("ce-2h16m",	"16 Kb Memory card");
    ExtArray[ID_CE2H32M]= new CExtension("ce-2h32m",	"32 Kb Memory card");
    ExtArray[ID_CE2H64M]= new CExtension("ce-2h64m",	"64 Kb Memory card");

    ExtArray[ID_CE1600M]= new CExtension("ce-1600m",	"32 Kb RAM Module");
    ExtArray[ID_CE1601M]= new CExtension("ce-1601m",	"64 Kb RAM Module");
    ExtArray[ID_CE1620M]= new CExtension("ce-1620m",	"32 Kb Eprom Module");
    ExtArray[ID_CE1625M]= new CExtension("ce-1625m",	"32 Kb Eprom Module");

    ExtArray[ID_CE16096]= new CExtension("ce-16096",	"96 Kb custom RAM Module");
    ExtArray[ID_CE16128]= new CExtension("ce-16128",	"128 Kb custom RAM Module");
    ExtArray[ID_CE16160]= new CExtension("ce-16160",	"160 Kb custom RAM Module");
    ExtArray[ID_CE16192]= new CExtension("ce-16192",	"192 Kb custom RAM Module");
    ExtArray[ID_CE16224]= new CExtension("ce-16224",	"224 Kb custom RAM Module");
    ExtArray[ID_CE16256]= new CExtension("ce-16256",	"256 Kb custom RAM Module");



    for (int i = ID_CE16256+1;i<30;i++)
            ExtArray[i]= new CExtension();


//    ExtList << CExtension("ce-151",		"4 Kb memory module")
//            << CExtension("ce-153",		"Graphic pad")
//            << CExtension("ce-155",		"8 Kb memory module")
//            << CExtension("ce-156",		"Kanji characters tape")
//            << CExtension("ce-157",		"Kanji characters support module")
//            << CExtension("ce-158",		"Centronic and RS-232 interface")
//            << CExtension("ce-159",		" 8 Kb lithium battery backup")
//            << CExtension("ce-160",		"16 Kb lithium battery backup")
//            << CExtension("ce-161",		"16 Kb lithium battery backup")
//            << CExtension("ce-201m",		" 8 Kb Memory card")
//            << CExtension("ce-202m",		"16 Kb Memory card")
//            << CExtension("ce-203m",		"32 Kb Memory card")
//            << CExtension("ce-210m",		" 2 Kb Memory card")
//            << CExtension("ce-211m",		" 4 Kb Memory card")
//            << CExtension("ce-212m",		" 8 Kb Memory card")
//            << CExtension("ce-2h16m",	"16 Kb Memory card")
//            << CExtension("ce-2h32m",	"32 Kb Memory card")
//            << CExtension("ce-2h64m",	"64 Kb Memory card")
//
//            << CExtension("ce-1600m",	"32 Kb RAM Module")
//            << CExtension("ce-1601m",	"64 Kb RAM Module")
//            << CExtension("ce-1620m",	"32 Kb Eprom Module")
//            << CExtension("ce-1625m",	"32 Kb Eprom Module")
//
//            << CExtension("ce-16096",	"96 Kb custom RAM Module")
//            << CExtension("ce-16128",	"128 Kb custom RAM Module")
//            << CExtension("ce-16160",	"160 Kb custom RAM Module")
//            << CExtension("ce-16192",	"192 Kb custom RAM Module")
//            << CExtension("ce-16224",	"224 Kb custom RAM Module")
//            << CExtension("ce-16256",	"256 Kb custom RAM Module");




}

void CExtensionArray::setAvailable(int ind,bool value)
{
    (ExtArray[ind])->IsAvailable = value;
}



void CExtensionArray::setAvailable(int ind,bool value,bool checked)
{
    (ExtArray[ind])->IsAvailable = value;
    if (checked) this->setChecked(ind,checked);
}

void CExtensionArray::setChecked(int ind,bool value)
{
    for (int i = 0;i<30;i++)
        ExtArray[i]->IsChecked = false;

    ExtArray[ind]->IsChecked = value;
}


