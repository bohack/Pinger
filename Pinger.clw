; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=HelpDialog
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Pinger.h"

ClassCount=3
Class1=CPingerApp
Class2=CPingerDlg

ResourceCount=3
Resource1=IDR_MAINFRAME
Resource2=IDD_PINGER_DIALOG
Class3=HelpDialog
Resource3=IDD_HELP

[CLS:CPingerApp]
Type=0
HeaderFile=Pinger.h
ImplementationFile=Pinger.cpp
Filter=N
LastObject=CPingerApp

[CLS:CPingerDlg]
Type=0
HeaderFile=PingerDlg.h
ImplementationFile=PingerDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_LIST



[DLG:IDD_PINGER_DIALOG]
Type=1
Class=CPingerDlg
ControlCount=29
Control1=IDC_STATIC,static,1342308352
Control2=IDC_TIMEOUT,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_PASSES,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_O1,edit,1350639744
Control8=IDC_O2,edit,1350639744
Control9=IDC_O3,edit,1350639744
Control10=IDC_O4,edit,1350639744
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_O5,edit,1350639744
Control16=IDC_O6,edit,1350639744
Control17=IDC_O7,edit,1350639744
Control18=IDC_O8,edit,1350639744
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352
Control21=IDC_STATIC,static,1342308352
Control22=IDC_SCAN,button,1342242816
Control23=IDC_STATUS,static,1342308352
Control24=IDC_CLEAR,button,1342242816
Control25=IDC_COPY,button,1342242816
Control26=IDC_SAVE,button,1342242816
Control27=IDC_ABOUT,button,1342242816
Control28=IDC_LIST,listbox,1352728961
Control29=IDC_RESOLVE,button,1342242819

[DLG:IDD_HELP]
Type=1
Class=HelpDialog
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352

[CLS:HelpDialog]
Type=0
HeaderFile=HelpDialog.h
ImplementationFile=HelpDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=IDOK

