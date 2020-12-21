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

#define BPM     120         //Beats Per Minute
#define QN      60000/BPM   //4分音符(quarter note) の長さ(60000ミリ秒=1分)

int music[][3] = {          //ドレミ
        {OC,DO,QN},
        {OC,RE,QN},
        {OC,MI,QN},
        {OC,FA,QN},
        {OC,SO,QN},
        {OC,RA,QN},
        {OC,SI,QN},
        {OH,DO,QN},
        {OC,REST,QN},       //曲の最後は休止符を挿入して音を止める
};
