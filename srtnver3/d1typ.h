typedef struct
{

  double av;              //
  double avc;             //
  double azoff;           //
  double azcounts_per_deg;//
  double azlim1;          //
  double azlim2;          //
  double aznow;           // current azimuth
  double azcmd;           //
  double avbsw;           //
  double azprev;          //
  int azelsim;            // simulates antenna
  int azcount;            //
  int azelport;           //

  double bw;              //
  double beamw;           // beam width
  double bswpwr;          //
  int bsw;                //
  int bswint;             //

  double calcons;         //
  double calpwr;          //
  int clearint            //
  int countperstep;       //
  int calon;              //
  int calmode;            //
  int caldone;            //
  int comerr;             //
  int cmdfl;              //
  int cmdfline;           //
  char cmdfnam[256];      //
  char catnam[64];        //

  double dmax;            //
  double delay;           //
  int displ;              //
  int debug;              //
  int domap;              //
  int drift;              //
  int docal;              //
  int dongle;             //
  int devices;            //
  char datadir[256];      //

  double efflofreq;       //
  double eloff;           //
  double elcounts_per_deg;//
  double ellim1;          //
  double ellim2;          //
  double elnow;           // current elevation
  double elprev;          //
  double elcmd;           //
  int elcount;            //
  int entry1;             //
  int entry2;             //
  int entry3;             //
  int entry5;             //
  int entry6;             //
  int entry8;             //

  double freq;            // frequency to observe at
  double f1;              //
  double fc;              //
  double f2;              //
  double fbw;             // bandwidth set in srt.cat
  double freqcorr;        //
  int foutstatus;         //
  int freqchng;           //
  int fstatus;            //
  int fftsim;             // simulates fast-fourrier transform
  char filname[256];      //

  double glat;            //
  double glon;            //

  double hgt;             //
  int helpwindow;         //
  char hlpnam[64];        //

  double integ;           //
  double integ2;          //
  double integ3;          //
  double intg;            //
  int id;                 //

  double lat;             // value for latitude
  double lon;             // value for longitude
  double lofreq;          //
  int limiterr;           //

  int mainten;            //
  int map;                //
  int mancal;             //

  double noisecal;        //
  int nsou;               //
  int nsecs;              //
  int nfreq;              //
  int newday;             //
  int nsecstart;          //
  int nbsw;               //
  int nblock;             //
  int nsam;               //
  int numon;              //
  int numoff;             //
  int nrfi;               //
  int npoly;              //

  int obsn;               //

  double pwroff;          //
  double pwron;           //
  int ppos;               //
  int ptick;              //
  int plotsec;            //
  int printout;           //
  int ptoler;             //
  int psw;                //
  int plot;               //

  double restfreq;        //
  double rod1;            //
  double rod2;            //
  double rod3;            //
  double rod4;            //
  double rod5;            //
  double rfisigma;        //
  double rfi[25];         //
  double rfiwid[25];      //
  int record_int_sec;     //
  int record_spec;        //
  int record;             //
  int rday;               //
  int radiosim            // simulates reciever
  int rod;                //
  int run;                //

  double start_time;      //
  double scnazoff;        //
  double scneloff;        //
  double smax;            //
  double secs;            //
  int speed_up;           //
  int secstop;            //
  int south;              //
  int scan;               //
  int stow;               //
  int slew;               //
  int sourn;              //
  int stopproc;           //
  unsigned int seed;      //
  char statnam[32];       //

  double tsys;            //
  double tant;            //
  double tload;           //
  double tspill;          //
  double tcal;            //
  int track;              //
  long tstart;            //
  char timsource[8];      //

  int unitid;             //

  double vlsr;            //
  int vwid;               //
  int vwhgt;              //
  int vwindow;            //

  int wid;                //
  int whgt;               //

  int xmark;              //

  double yfac;            //
  int year;               //


} d1type;
