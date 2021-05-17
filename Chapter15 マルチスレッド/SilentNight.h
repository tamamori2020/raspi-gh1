#define OL      3   //1 オクターブ下
#define OC      4   //中央のオクターブ
#define OH      5   //1 オクターブ上

#define REST    0   //休符
#define DO      1   //ド
#define DOS     2   //ド#
#define RE      3   //レ	
#define RES     4   //レ#
#define MI      5   //ミ	
#define FA      6   //ファ
#define FAS     7   //ファ#
#define SO      8   //ソ	
#define SOS     9   //ソ#
#define RA      10  //ラ
#define RAS     11  //ラ#
#define SI      12  //シ	

#define BPM     200         //Beats Per Minute
#define QN      60000/BPM   //4分音符(quarter note) の長さ(60000ミリ秒=1分)

int music[][3] = {          // Silent Night
        {OC,SO,QN*3},
        {OC,RA,QN},
        {OC,SO,QN*2},
        {OC,MI,QN*6},
        {OC,SO,QN*3},
        {OC,RA,QN},
        {OC,SO,QN*2},
        {OC,MI,QN*2},
        {OC,REST,QN},

        {OH,RE,QN*4},
        {OH,RE,QN*2},
        {OC,SI,QN*6},
        {OH,DO,QN*4},
        {OH,DO,QN*2},
        {OC,SO,QN*4},
        {OC,REST,QN*2},

        {OC,RA,QN*4},
        {OC,RA,QN*2},
        {OH,DO,QN*3},
        {OC,SI,QN},
        {OC,RA,QN*2},
        {OC,SO,QN*3},
        {OC,RA,QN},
        {OC,SO,QN*2},
        {OC,MI,QN*4},
        {OC,REST,QN*2},

        {OC,RA,QN*4},
        {OC,RA,QN*2},
        {OH,DO,QN*3},
        {OC,SI,QN},
        {OC,RA,QN*2},
        {OC,SO,QN*3},
        {OC,RA,QN},
        {OC,SO,QN*2},
        {OC,MI,QN*4},
        {OC,REST,QN*2},

        {OH,RE,QN*4},
        {OH,RE,QN*2},
        {OH,FA,QN*3},
        {OH,RE,QN},
        {OC,SI,QN*2},
        {OH,DO,QN*6},
        {OH,MI,QN*4},
        {OC,REST,QN*2},


        {OH,DO,QN*2},
        {OC,SO,QN*2},
        {OC,MI,QN*2},
        {OC,SO,QN*3},
        {OC,FA,QN},
        {OC,RE,QN*2},
        {OC,DO,QN*8},
        {OC,REST,QN*2},

        {OC,REST,QN*6},       //曲の最後は休止符を挿入して音を止める

};
