typedef struct
{
  double lat;
  double lon;
  double hgt;
  double integ;
  double integ2;
  double integ3;
  double dmax;
  double freq;
  double lofreq;
  double efflofreq;
  double vlsr;
  double start_time;
  double av, avc, glat, glon, intg, restfreq, bw, delay, azoff, eloff, scnazoff, scneloff, calcons, beamw, smax, azcounts_per_deg,
    elcounts_per_deg, rod1,rod2,rod3,rod4,rod5, azlim1, azlim2, ellim1, ellim2, tsys, secs, tant,tload, tspill, aznow, elnow, azcmd, elcmd, 
    pwroff, pwron, bswpwr, noisecal, avbsw, calpwr, yfac, f1, fc, f2, fbw, tcal, freqcorr, rfisigma, azprev, elprev;
  double rfi[25];
  double rfiwid[25];
  int speed_up;
  int nsou;
  int foutstatus;
  int ppos;
  int ptick;
  int nsecs;
  int plotsec;
  int displ;
  int printout;
  int debug;
  int nfreq;
  int record_int_sec;
  int record_spec;
  int record;
  int newday;
  int rday;
  int freqchng;
  int track;
  int nsecstart;
  int secstop, clearint, fstatus, radiosim, azelsim, mainten, azcount, elcount, domap, xmark, map, mancal, south, ptoler, countperstep,
    azelport, drift, scan, stow, slew, sourn, bsw, nbsw, bswint, calon, calmode, docal, caldone, rod, stopproc, comerr, limiterr, 
    cmdfl, cmdfline, year, nblk, nsam, unitid, run,wid,whgt,vwid,vwhgt,numon,numoff,psw,entry1,entry2,entry3,entry5,entry6,entry8,
    helpwindow,vwindow,plot,obsn,nrfi,dongle,npoly;
  int fftsim;
  unsigned int seed;
  int devices;
  int id;
  long tstart;
  char filname[256];
  char cmdfnam[256];
  char datadir[256];
  char catnam[64];
  char hlpnam[64];
  char statnam[32];
  char timsource[8];
} d1type;
