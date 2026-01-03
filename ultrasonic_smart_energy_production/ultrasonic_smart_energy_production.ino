/*
  ULTRASONIC SMART ENERGY PRODUCTION - ESP32 TYPE-C
  Kombinasi sensor ultrasonic dengan sistem lengkap:
  - WiFi & HTTPS hosting integration
  - TFT Display
  - AC Control (IR)
  - DHT22 & LDR sensors
  - Ultrasonic people counter (mengganti proximity)
  
  Hardware:
  - ESP32 Type-C
  - 2x HC-SR04 Ultrasonic Sensor (mengganti proximity)
  - TFT Display
  - DHT22 Temperature & Humidity
  - LDR Light sensor
  - IR LED untuk AC control
*/

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

// ================= TFT =================esponse
}or faster ray fain loop del Reduced m //; (50)elay d
   );
  }
is(mill = heckstWiFiC la
    }
   ();tWiFiec
      conn.."); kembali.mbung menya"WiFi.println(    Serial{
  ECTED) NNCOs() != WL_(WiFi.statu   if  {
 15000)iFiCheck > stWllis() - la  if (miCheck = 0;
 lastWiFisigned longtic unnt)
  starequemore fnds - y 15 secoevercted (check sconneWiFi if di/ Reconnect 
  
  /);
  }sendReasonToAPI( sendData{
   d) uldSen if (sho
 dedee if nostingta to h// Send da 
  }
 
  };
    + "s)"/1000) TEWITHOUT_UPDAX_TIME_+ String(MA" te paksa (pda = "UonasndRe   se;
   dSend = true    shoul {
  e())nSendUpdatif (ca
    pdate()) {ldForceUf (shou} else i}
  
    ;i" terdeteksbahanon = "PerueasendR      s= true;
uldSend 
      shote()) {ndUpdaanSe if (c()) {
   ngeicantCha (hasSignif
  if "";
  eason =ring sendRse;
  Stfalend = l shouldSboosting
  o ho tatald send d we shouk if  // Chec}
  
  millis();
eck = ChntrolACCo);
    lastontrolAPI(ckACC {
    cheL)NTERVAL_CHECK_IC_CONTRO= ArolCheck >CConttA) - lasif (millis(
   frequent)onds - morevery 5 sechosting (ends from commaontrol C check for A  // Csor();
  
aSenampilkanDat  t berkala
araensor sec data sTampilkan
  // ;
  }
  ngjumlahOraOrang = lastJumlah  Ruang);
  suhuta., currentDa, setTempatusateTFT(acSt {
    updng)ahOra= lastJumlhOrang !  if (jumla
ngedle count cha TFT if peop/ Update 
  /setTemp;
 = Temp Data.setnt;
  curre acStatusa.acStatus = currentDattTemp);
 atus, seolAC(acStp;
  kontra.setTemcurrentDatemp =   int setTtus;
a.acStarentDatStatus = curing acng)
  Strm hostimanual froauto or C (ol AContr  // a();
  
nsorDatupdateSeorang
  teksi  Logic de//);         c(aUltrasonibacy()
  mitoxiaPrbac Mengganti nsors();  //SetrasoniceadUl rROXIMITY
  PEAD OFC INSTRASONIons - ULTnctiain fu  
  // M
    }
  }======");
========================println("🆘 Serial.
      an ini");pilkan bantu Tamn("help/h -ntlerial.pri
      S);(0-20)" orang ke X Set jumlah"set X - intln(Serial.pr);
      ini"s saat  Statu -s/s"statuntln(al.priSeri
      etail");nsor dstatus sean Tampilkt - n("test/rial.printl    See 0");
  rang k oeset jumlahr - R"reset/println(  Serial.    ");
 ===RSEDIA YANG TEERINTAH P("🆘 ===tln Serial.prin   "h") {
  == nd maom || chelp"= " (command =   else if    }
   }
0");
    kan 0-2. Gunaak validmlah tidintln("❌ Jul.pr   Seria  lse {
   } e
      );lahOrang)ring(jum+ Stunt: "  set coPI("ManualendDataToA    sa();
    DateSensor   updat
     ;rang))mlahOring(ju: " + Stet kerang dis✓ Jumlah o.println("     Serialnt;
   ang = newCou   jumlahOr
     0) {ount <= 2>= 0 && newCCount     if (new
  );g(4).toInt(nd.substrin commaount =   int newC")) {
   "set th(sWid.startommanse if (c }
    el\n");
   =======================📊 =println("l.Seria
      ;
      "(Kosong)")" : k)a obje ? "(AdtOutDetectedjecData.obasonicn(ultrrial.printlSe);
      m "al.print("c     Serit);
 a.distanceOuasonicDat.print(ultrial    Ser");
  r KELUAR: "🔍 Sensoial.print(    Ser      
  )");
song: "(Koek)"  objAdaected ? "(etjectInDicData.obtln(ultrasonerial.prin S);
     nt("cm "al.prieri S
     eIn);Data.distanc(ultrasonical.printSeri");
      UK: AS Sensor M"🔍.print(  Serial      
    20");
println("/al.riSe    );
  (jumlahOrangintpral.eri
      S); ruangan: "ng dalam👥 Jumlah oraprint("ial.   Ser");
   ===AT INI TUS SATA=== S"\n📊 .println(ialer   S
    "s") {and ==" || comm= "status(command =  else if 
  ");
    }====================== ======println("🔍    Serial.
      
  n();.printlerial}
      S);
       dBm""int(erial.pr.RSSI()); Srint(WiFi Serial.p);RSSI: "nt(" | erial.pri      S;
  g())P().toStrini.localInt(WiFrial.pri"); Se(" | IP: rintl.pia     SerD) {
   CONNECTE) == WL_tatus(f (WiFi.s i");
     rputus ❌✅" : "Te"Terhubung NECTED ? _CONWLtatus() == t(WiFi.s Serial.prin    ; 
  WiFi: ")int("📶al.pr     Seritatus);
 ata.acS(currentDtln.prinal "); SeriAC:atus "❄️ Sterial.print(      S;
Orang)n(jumlah.printlialng: "); Serlah ora"👥 Jum.print( Serial    
 tem Status Sys  //  
    ;
    .println()erial S");
     int(" ❌erial.prse S ✅"); elal.print(" SeridrRaw > 0)    if (l");
   luxprint("erial.drMapped); Sial.print(l"); Sered: ppt(" | Ma Serial.prin  
   ldrRaw);l.print(; Seriaw ADC: ")   Ra"\nrint(.p      Serial 34):");
DR (GPIO Sensor L.print("💡erial  S   1000);
  5, 0,rRaw, 0, 409p(ld= maMapped ldr
      int ;PIN)ead(LDR_analogR ldrRaw =       intnsor
 LDR Se  // Test  
        );
.println(     Serial}
       
");k terbaca ❌RROR - Tidant("Eri   Serial.p   } else {
  );
      t("% ✅"in.prst); Serial(humTerint    Serial.p   ) {
 est)!isnan(humTf (  i   );
 idity: "("\n   Humerial.print  S
    
      }ca ❌"); terba- TidakR int("ERRO  Serial.pre {
         } els");
   ("°C ✅al.print SeriempTest);rint(tal.p Seri
       Test)) {(tempf (!isnan);
      iu: "  Suhn rint("\l.peria      SO 27):");
 (GPIr DHT22nso"🌡️ Sel.print(   Seria
   ;idity().readHumdht= humTest float    
   re();emperatudht.readT = stfloat tempTesor
      t DHT22 Sen   // Tes     
   ln();
 erial.print   S   }
     KSI");
 NGE DETE RA ⚠️ DI LUARal.print(" Serie {
       els  } R");
    ROERGE atau UT OF RANrint(" ❌ Oerial.p  S    == 0) {
  dist2  else if ( }
     DETEKSI");RANGE ALAM t(" ✅ D.prin      Serial
  ) {_DETECTIONst2 <= MAXE && diMIN_DISTANC=  (dist2 >);
      if"cm"t(rial.prinSe
      ;ist2).print(dial    Ser: ");
  7)IO 16/1 (GPARLUr KEso🔍 Sen.print("      Serial      
rintln();
 Serial.p  
     }
    I");RANGE DETEKS ⚠️ DI LUAR int("al.prSeri        else {
   }");
    RRORatau EUT OF RANGE  ❌ Oint(".prial        Ser== 0) {
e if (dist1       } els");
GE DETEKSIAN R✅ DALAMal.print("         SeriECTION) {
<= MAX_DET& dist1 DISTANCE & >= MIN_ if (dist1
     ");("cminterial.pr;
      Sist1)int(dial.prSer     5): ");
 O 2/1 (GPIsor MASUK Senrint("🔍al.pri   Se
    
     T);IN_OUT, ECHO_PN_OU(TRIGGER_PIonicDistancerasdUltt2 = reais int d(50);
       delay
    ;PIN_IN)_IN, ECHO_INe(TRIGGER_PicDistancadUltrasonre1 = ist     int dors
 rasonic Sens Test Ult //   
     ");
   DETAIL ===ATUS SENSOR "🔍 === STrintln(rial.p   Se
    { == "t")|| command" estd == "t if (comman
    else    }: 0");
t - Countual rese"ManToAPI(dData sen   
  a();sorDatSen      update");
eset ke 0 orang dir- Jumlah"✓ Reset (rial.println  Se;
    ected = 0nDeterso    lastP
  ate = IDLE;tStren;
      curng = 0hOrajumla     ") {
  == "randmmset" || coand == "re   if (comm;
    
 ()Case.toLowermandom c;
   ()rimand.tmm
    cotil('\n');eadStringUnal.reriommand = SString c) {
    lable()l.avairia
  if (Seal commandsseri/ Check for oop() {
  /===
void l=======LOOP ========== MAIN ==========}

// ====\n");
==========================n("======.printl Serial===");
 ==================🔍 =========ntln("Serial.pri");
  k/keluarg masuksi oranuk dete siap untln("Sensornt Serial.prigi");
 akurasi ting10cm untuk 1-deteksi: ("Range printlnerial.
  Slesai!"); seultrasonic"✅ Test ntln(riial.p Ser;
 TEST:")IL "\n📊 HASl.println(riaSe 
  }
 0);
    delay(5    
  n();
.printl    Serial
;
    } ***")ERDETEKSI! OBJEK Tt(" ***l.prinria{
      Se) CTION)MAX_DETEist2 <= CE && dIN_DISTANt2 >= M    (dis| 
    ) |IONTECTAX_DE= M dist1 <CE &&N_DISTANdist1 >= MI  if ((
    m");
  ist2) + "cring(dSt " + ("KELUAR:al.printeri ");
    S1) + "cm |ing(dist" + Str"MASUK: int(  Serial.pr
  0 - "); + "/5(i+1) " + Stringt("Testprin    Serial.;
    
y(25));
    delaCHO_PIN_OUTR_PIN_OUT, ETRIGGEstance(icDionrasdUlt= reant dist2 );
    iy(25  dela_IN);
  INO_PPIN_IN, ECHIGGER_(TRicDistanceeadUltrason rst1 =   int di++) {
  < 50; i 0; i(int i =  
  for n");
ubahaihat perk mel sensor untubjek kekan oa dekatob"Cprintln(ial. Ser);
 tik..."dema 5 or selaest sens t("Melakukanintln
  Serial.pr"); ===ONIC DETAILULTRASTEST SENSOR = ==ntln("\n🔍 l.pri SeriaETAILED
  & DNHANCED Ers -soasonic sen// Test ultr
  
  );an"ntuh' - Bau 'p' ata 'hel"-ntln(  Serial.pri ini");
attatus sa - Sau 's'atatus' - 'st"ln(ntSerial.priX");
   ke  oranglah Set jumset X' -- 'ntln(" Serial.priail");
  detortatus sensu 't' - Sest' ata'tprintln("- 
  Serial.ng");umlah oraeset j- R' atau 'r'  'reset("-rintln Serial.p;
  Serial:")Perintahln("\nintl.pr  
  Seria;
20 orang")mal tas maksipasitln("- Kaal.prinerick");
  Sallbagai fomatis sebaode otintln("- Mal.pr);
  Serihboard web" via dasalC manuontrol Antln("- KSerial.prition");
  h detecaraSR04 untuk C- 2 sensor H.println("-
  Serialonic");ultrasdengan uar asuk/keli orang m- Deteksntln("ial.prii");
  Seroduksg prasi hostingrntentln("- Ierial.pri S);
 HTTPS/SSL"munikasi "- Ko(.println Serial;
 ur:")intln("Fitial.prer");
  SM SIAP!ln("SISTEerial.print);
  
  S = millis(CheckstACControl);
  las(date = milliorceUplastF
  e timingiztialni
  
  // IectWiFi();conno WiFi
  nect ton/ C
  /UI();
  
  drawon(1);ati tft.setRotit();
 .in  tft TFT
zeInitiali/ ();
  
  /gin
  ac2.be.begin();  ac1in();
t.begors
  dhze sens// Initiali;
  
  T, LOW)ER_PIN_OUlWrite(TRIGGgita  diOW);
ER_PIN_IN, Lite(TRIGGitalWr  dig;
  
_PIN, INPUT)e(LDRnModNPUT);
  piT, I_OUe(ECHO_PIN pinMod
 T);UTPU_OUT, OTRIGGER_PIN
  pinMode(, INPUT);N_INMode(ECHO_PIinTPUT);
  pN, OUER_PIN_Iode(TRIGG  pinMIC SENSORS
TRASONns - ULitialize pi  // In");
  
===============================ntln("=ril.p Seriag");
  20 oranimal:tln("  Maksprinl.ia");
  Serggi)(Presisi Tinnge: 1-10cm "  Ra.println();
  Serialc"soniltra2x HC-SR04 U: nsortln("  Seal.prin Serian");
 iaktifk DSSL/HTTPS:rintln("   Serial.pd");
 t.unja.ac.i: dasko.fsainDomprintln("  rial. SeSION");
 TRASONIC VER - ULNERGY"  SMART Entln(priial.=");
  Ser==============================="\nln(erial.print  S;
  
ay(2000)del;
  (115200)beginl.
  Seria {tup()d se======
voi========== SETUP =============
// =====
0;
  }
}I = wifiRSSrrentData. cu  ed";
 "DisconnectiStatus = ata.wifntD{
    currelse  } e
 iFi.RSSI();SSI = Wta.wifiRrentDa
    curd";= "ConnectewifiStatus Data.
    currentcted()) {.isConne(WiFi
  if .0;
  = 0a.humidity urrentDat c.humidity))tDatacurren if (isnan(
  = 0.0;huRuangrrentData.sung)) cusuhuRuaata.nan(currentDf (is
  i);
  p = millis(.timestamrrentData;
  cu000)95, 0, 10, 40DR_PIN), alogRead(Lvel = map(anightLea.l currentDatmidity();
 Huready = dht..humiditurrentData;
  cemperature()t.readTg = dhuRuantData.suhenurrOrang;
  cah = jumlOrang.jumlahntDatare
  cur() {teSensorDatapda==
void u============R DATA ===NSOATE SE=== UPD============
// ==
  }
}
p2;rgetTem tatTemp2 = las
   1;mpTergetTemp1 = ta   last ac2ON;
 tAC2 = lasON;
   stAC1 = ac1
    la  }
        }
  .send();
  (); ac2ff  ac2.o
      lse {     } e.send();
      ac2p2);
   argetTem(t2.setTemp     ac   
nicAcCool);soMode(kPana ac2.set();
       c2.on        a) {
f (ac2ON   
      i}
         ;
nd()); ac1.se1.off(   ac
     lse { e;
      }.send()c1 a);
       (targetTemp11.setTemp  ac    ;
  cAcCool)(kPanasoni1.setMode  ac      .on();
     ac1 {
   ac1ON)if (  
      } else {nd();
  .se; ac22.off()     ac.send();
 c1ff(); a      ac1.o !ac2ON) {
c1ON &&
    if (!a);
     "°C"mp) ++ String(te" @ " tus + : " + sta AC"Perubahanln(l.print{
    Seria lastTemp2) rgetTemp2 !=p1 || talastTemtTemp1 != C2 || targe= lastA2ON !ac lastAC1 ||  !=1ON
  if (ac differenty ifonlges pply AC chan
  
  // A };
    }
 MAX"2 AC = "s tuta    s 20;
  temp =    0;
  etTemp2 = 2     targ20;
 mp1 = Te     targetN = true;
 
      ac2ON = true;    ac1O people
   max 20d forte // Upda<= 20) { ahOrang  if (juml  else
    }
  "; ONs = "2 AC   statu  
 mp = 22;    te2;
  = 22 mptargetTe
      1 = 22;  targetTemp
    = true;  ac2ON     true;
 ON =c1 {
      a5)ang <= 1 (jumlahOrse if  }
    el   AC ON";
"1tus = ta22;
      s  temp = 22;
    rgetTemp1 = 
      taue;trac1ON = 
      0) {g <= 1 (jumlahOran if}
    else";
    ON AC status = "125;
      mp = 
      te25;mp1 = targetTe
      ;rue1ON = t      ac <= 5) {
ng (jumlahOra  else if
    }
    temp = 0;FF";
    tus = "AC O     stag == 0) {
 lahOranf (juml) {
    itrooniveCtrol.hasActde || !acConrierrol.manualOvCont
  if (!acal expires)en manu or whfaultuto mode (de
  // A  }
      }
"auto";
olMode = ontrol.contr  acCe;
    rol = falsveConthasActi  acControl.  
   false;erride =nualOvol.ma acContr");
     e otomatis modli kekembarsa, uwaal kedi hostingl manual darontro"Kal.println( Serito
      au tourned, retntrol expiral co/ Manu  /
    e { } els
   ); + "°C"tring(temp) " @ " + S + status +osting: "ri h manual dal AC("Kontrorintlnal.p
      Seri
      ;
      }emp2getTartTemp1 : tN ? targeemp = ac1O;
        t"MANUAL 1 ACstatus = " {
        } else
       2;tTemp2) /rgemp1 + taTeetp = (targtem       L 2 AC";
 MANUAstatus = "      ON) {
   ac2c1ON &&se if (a     } elmp = 0;
       te;
  FF"NUAL Otatus = "MA     sN) {
    && !ac2O (!ac1ON  
      ifture;
    l.ac2TemperacControTemp2 = arget  ta
    re;1TemperatuaccControl.mp1 = atargetTe     atus;
 l.ac2St acContro    ac2ON =atus;
  ol.ac1Stontr= acC   ac1ON ting
   s from hossettingl l controuaUse man
      // resAt) {trol.expi < acCon || millis()t == 0xpiresAacControl.e {
    if (ontrol)asActiveC acControl.hlOverride &&uaol.mancContr if (at expired
 nd nos active al override ieck if manua // Ch
   ;
= 25targetTemp2  int  25;
 etTemp1 =t targ;
  inc2ON = falseool a
  b false; =l ac1ON  booemp) {
int &tus,  &statlAC(String
void kontro=============NTROL ==== COAC========== 
// =======}
}
= now;
  Display Sensor last);
   \n"===================📊 ==========.println("ial Ser  
   ();
  rial.println
    Se  }
  ");m pernahBelunt(" Serial.pri{
     
    } else lalu"); yang " detik.print(   Serial000);
    1 /APIUpdate)astw - l(noint(pral.eri
      Sdate > 0) {IUpstAP    if (ladate: ");
Last API Up   nt("\n.prial   Series");
 " bytprint(    Serial.eap());
SP.getFreeH.print(Eal  Seri: ");
   | Free Heap menital.print("  Seri  
1000) / 60);rint((now / Serial.p");
    me: tiUp   "\nprint(ial.  Serth:");
   System Heal("⚙️Serial.printlth
    ystem Hea/ S  /   
  
 .println();erial    S");
    }
t("°Crinl.p  Seria
    mp);setTeurrentData.rint(cerial.p);
      St(" @ "erial.prin{
      S 0) Temp >entData.set (currif   s);
 ata.acStatu(currentDerial.print");
    Sus: n   Statnt("\rial.priSe
    ");
    } 🤖)" (AUTOl.print(ria {
      Se } else);
   int(")"   Serial.pry);
   atedBol.crecContral.print(aSeri);
      ️ oleh: " (MANUAL 🎛rint(".p  Serialde) {
    alOverrimanucControl.
    if (aolMode);ntrControl.corint(ac   Serial.pe: ");
 Modn   print("\
    Serial.trol:");"❄️ AC Conint( Serial.prntrol
   Coa AC  Dat
    //
    .println();  Serial
    }
  ECTED");❌ DISCONNprint("   Serial. {
        } elseOR 📶)");
print(" (POal.lse Seri
      e);" (FAIR 📶)"al.print(70) SeriSI > -iRSData.wif(currentlse if    e");
   " (GOOD 📶)l.print(eria) SI > -60Data.wifiRSS (current     else if📶)");
 T XCELLEN" (Ent(al.pri0) SeriifiRSSI > -5ata.wcurrentD   if (;
   ")print(" dBmerial.
      SSSI);ntData.wifiRprint(curre Serial.    );
 " RSSI: "\n  .print( Serial  
   toString());ocalIP().rint(WiFi.lerial.p     SIP: ");
 ✅ | int(" ial.pr    Ser {
  TED)WL_CONNEC == tus()iFi.sta if (W);
   iStatustData.wift(currenrin Serial.p ");
   s:Statut("\n   ial.prin");
    Serection:"📶 WiFi Connal.print(eriyal
    Slitas sinn kua dengaa WiFi// Dat
    
    rintln();al.peri
    S");(RAMAI 👫👬👭)t(" l.prinse Seria el");
   (SEDANG 👥)int(" rial.pr= 15) Seg <ahOranjumlif (se    el);
 KIT 👤)"" (SEDIal.print(5) Serig <= (jumlahOrane if   els)");
  OSONG 🏢 (Knt(" Serial.priOrang == 0)if (jumlah  ");
  0nt("/2rial.pri Se   ahOrang);
(jumlintial.prer    Sg: ");
mlah Oran"👥 Jul.print(   Seriastatus
 n  Orang denga Jumlah
    // Data);
    ln(ial.print   Ser🌙)");
 LAP GEint(" (rial.prelse Se   );
 DUP 🌥️)"(RE.print(" 0) Serialevel > 10ightLentData.l (currlse if e)");
   ANG 🌤️rint(" (SEDal.priel > 300) Seeva.lightLDatntf (curre    else i️)");
ERANG ☀t(" (TSerial.prinvel > 700) Lehtata.ligrentDcur (if);
    " lux"(al.print   Seri
 vel);ata.lightLetDprint(currenrial.");
    Seapped: rint(" | Mal.p   SeriawLDR);
 nt(r  Serial.pri);
   " ADC:"\n   Rawint(Serial.pr");
    R): (LD"💡 Cahayaerial.print(  S;
  _PIN)ogRead(LDR = analt rawLDR inretasi
   erpan int) dengvel(Light LeDR  Data L
    //    println();
Serial.      }
 a ❌");
 bac tidak tersor DHT22"ERROR - Senrial.print(    See {
  } els  
  )");MAL ✅int(" (NORl.pre Serials;
      eG 🏜️)")" (KERIN.print(erialdity < 30) Smia.hu(currentDate if 
      elsMBAB 💧)");" (LEial.print( Ser0)ty > 8midirentData.hu     if (cur");
 int("%   Serial.pr;
   .humidity)currentDataerial.print(      Sty > 0) {
dihumientData.urr
    if (cty: ");n   Humidi"\nt(rial.pri
    Se    ;
    }
baca ❌")idak teror DHT22 t Sens"ERROR -.print(ial Ser    } else {
 
     ✅)");ORMALrint(" (Nlse Serial.p  e;
    GIN ❄️)")(DINprint(" Serial.20) ang < Ruuhuta.stDarren else if (cu    )");
 " (PANAS 🔥t(Serial.prin 35) ang >tData.suhuRurrencu    if (");
  .print("°CSerial);
      huRuang.surentDatal.print(cur Seria 0) {
     huRuang >entData.su if (curr;
   ang: ")u Ruuh("\n   Sl.print  Seria
  an:"); Kelembabu &🌡️ Suh"nt(Serial.priasi
    ngan validHumidity deu &  Suh    // Data    
;
println()rial.");
    SeONG ❌): "(KOSI ✅)" ERDETEKSted ? "(TjectOutDetecata.obsonicDrint(ultraial.pSerm ");
    "cl.print(  Seriat);
  eOu.distanccDataoni(ultrasSerial.print");
    R: sor KELUA("\n   Senrial.print  Se
  ");"(KOSONG ❌)SI ✅)" :  "(TERDETEKtected ?tInDeta.objeccDat(ultrasoniin.pr  Serial;
  ("cm ")erial.print S
   anceIn);ata.distsonicDnt(ultraprierial.;
    SUK: ")ASor Mens   Snt("\nri Serial.p   ;
")c Sensors:trasoni("🔍 Ulerial.printty)
    Sroximi(mengganti p detail anc dengltrasoni U/ Data    
    /t");
ik sejak boontln(" det  Serial.pri0);
  / 100(now al.printSeri: ");
    ("⏰ Waktuint.pr Serialstamp
   timer dengan   // Heade  ");
    
ME ===L-TIEAENSOR RATA S"\n📊 === Dprintln(al.ri0) {
    Se00y >= 30nsorDispla- lastSeif (now k spam)
  tuk tida unlebih jarangetik ( dtiap 30 se sensorkan datapil  // Tam);
  
 millis(now =long ned iguns
   0; =playDislastSensorgned long static unsi
  or() {SensnDataampilka==
void t==============) =KI & LENGKAPRBAIPE(DIBERKALA NSOR TA SETAMPILKAN DA== ==============
}

// =);
  }te = millis(ftUpdalastT  
        }
  "Never");
println( tft. {
      } else   ");
gotln("s arint.p
      tf) / 1000);PIUpdate- lastAis() nt((mill tft.pri{
     0) pdate > f (lastAPIU
    i"); Update: print("Last;
    tft. 280)or(10,etCurs.s
    tft_WHITE);(TFTtColorsetTexs
    tft.// API statu
    4");
    nic HC-SR0 Ultraso("Sensor:tft.println
     265);sor(10,urft.setC tLLOW);
   _YEextColor(TFT  tft.setTr type
   Senso    //");
    
& VerifiedEnabled SL: println("S tft.
   , 250);etCursor(10 tft.s_CYAN);
   extColor(TFTsetTtft.s
    statu   // SSL 
 
    ja.ac.id");fst.undasko.Domain: t.println(");
    tf 2350,etCursor(1.s   tft
 TFT_GREEN);etTextColor(t.stfstatus
      // Domain    
  (1);
 tTextSize tft.se  LACK);
 , TFT_B 80 310,35,, 2illRect(10.f tftconds
   y 10 seer evte { // Upda10000)ate > tTftUpd las (millis() - = 0;
  ifdatelastTftUpgned long si  static unfrequent)
(less tus  state// Upda
  " C");
  t(
  tft.prinang);print(suhuRu0);
  tft.r(180, 18.setCursoft  t);
0, TFT_BLACK140, 3, ct(180, 175  tft.fillRe}
  
);
  int(" C"ft.pr);
    t(setSuhut.print    tf  else {
("-");
 tft.printSuhu == 0)  if (setLLOW);
TFT_YElor(xtCotTeft.se);
  t180, 140or(t.setCurs
  tfCK); TFT_BLA140, 30,0, 135, llRect(18  
  tft.fitatus);
t(acS  tft.prinN);
r(TFT_CYAsetTextColo tft.
 00);rsor(180, 1.setCu);
  tftetTextSize(2K);
  tft.sTFT_BLAC140, 30, ct(180, 95, llRe tft.fi
 
  ng);t(jumlahOrarin.p  tft_GREEN);
or(TFTColsetTexttft.80, 60);
  ursor(1tft.setCSize(3);
  ext
  tft.setTACK);TFT_BL5,  140, 380, 55,illRect(1tft.fues
  alain ve m
  // Updatang) { suhuRuatuhu, flosetStatus, int String acSpdateTFT(void u
}

SR04");rasonic HC-Sensor: Ult("intlnt.pr 265);
  tftCursor(10,.se");
  tftled"SSL: Enabntln(ft.pri0);
  tursor(10, 25  tft.setCc.id");
ja.afst.undasko.main: ("Do tft.println, 235);
 Cursor(10set;
  tft.g")on Hostinoductide: PrMontln(".pri20);
  tftsor(10, 2tft.setCur1);
  TextSize(ft.sets
  t // Statu");
  
  Ruang:hu"Sutft.println(;
  or(10, 180)tft.setCurs AC:");
  uhu("Set S tft.println0, 140);
 r(1rsoetCu
  tft.s);C Status:""Aft.println(
  t(10, 100);etCursor
  tft.sng:");"Jumlah Oraprintln( tft.);
 ursor(10, 60ft.setC
  t);TextSize(2tft.set
  
  RKGREY);, TFT_DA 40, 300Line(10,rawFastH.dft  tSONIC");
LTRAGY - UMART ENERn("Sft.printl 10);
  ttCursor(10,se
  tft.Size(2);tft.setTextK);
  TE, TFT_BLACFT_WHItColor(T.setTexACK);
  tft(TFT_BLenllScre
  tft.fi drawUI() {
void=================AY == TFT DISPL==============

// =
  }
}site");status webet dan ernsi inta konekks   Perin("al.printl    Seri
"); ke websiteta mengirim dak dapatGAL: Tida"❌ GA.println(ial
    Sere { } els
 );
    }int>())d"].as<"data"]["inseDoc[(respo " + Stringngan ID:mpan desite menyiebintln("✅ Wrial.prSe
      ]) {id""]["ata"d[ocseD && respon"]ssc["succe& responseDoerror &if (!    );
oc, responseonseDspson(rezeJiali deser =or errornErrtioliza   DeseriaeDoc;
 ns respoment<256>cuJsonDotatic    Sntuk ID
 unsespo re // Parse   
   s();
 e = millieUpdatlastForc    millis();
IUpdate = 
    lastAPData();reviousateP
    updiming data and tate previous// Upd 
    
   );se+ responnse: " poln("   ResSerial.print   
 ite!");m ke webs terkiritaRHASIL: Dantln("✅ BErial.pri)) {
    Seponsestr(), resg.c_nStrin jsoL, "POST",t(apiURPSRequesmakeHTT
  if (ng response;  
  Stri);
ring+ jsonSt " JSON:irim "📡 Mengn(rintlial.p
  
  SersonString);son(doc, jeJ serializring;
 jsonString 
  
  St= true;enabled"]  doc["ssl_omain;
 hostingD"] = g_domain"hostin
  doc[rride;lOvenuaol.maontre"] = acCnual_overridoc["ma
  dolMode;contracControl.l_mode"] = c["controdoupdate
  sensor_ingle_Mengganti s/ update";  /r_nic_sensosotra = "ulte_type"]updaoc["on;
  dreasson"] = update_reac["ive";
  doct "atatus"] =  doc["slis();
milstamp"] = doc["timeap();
  getFreeHeP. = ESe_heap"]
  doc["fre000;lis() / 1ime"] = mil  doc["upti.RSSI();
iF = Wi"]"wifi_rss
  doc[ity_sensorproximengganti sor;  // McSenltrasonientData.ur"] = currc_senso"ultrasonil;
  doc[ightLevetData.lcurren = level"]"light_ doc[idity;
 ata.hum] = currentDumidity""h[uang;
  docntData.suhuR] = curreemperature"oc["room_tp;
  dtData.setTemen] = currrature""set_tempeus;
  doc[Statata.acrentDs"] = cur["ac_statu docrang;
 ta.jumlahOurrentDat"] = ce_coundoc["peopl  OCATION;
= DEVICE_Lcation"] c["lodo
  VICE_ID;DEd"] = ["device_i;
  doct<700> docennDocumStaticJsoON data
   Buat JS);
  
  //ta.acStatusntDa" + curreAC: us   Statrintln(" 
  Serial.pg));ranmlahOData.jurenttring(currang: " + Slah Oln("   Jumal.print Seri
 eason);+ rasan: "    Alprintln("
  Serial....");websitea ke iman datan pengirpersiapk("📤 Memtlnial.prin
  
  Serurn;
  }ret   ");
 rhubungak te WiFi tidI -APm ke ri dapat mengiidak T.println("✗rial) {
    Se_CONNECTEDatus() != WL.stiFi(W") {
  if ian terdeteks = "Perubahreasoning ataToAPI(StrendD s=
void=========D =======- ENHANCEG API OSTINO H SEND DATA T================;
}

// =fiRSSIwirentData.rssi = curwifi_reviousData.r
  pnsoroximity_seti pnggan// Mer;  asonicSensoentData.ultrr = currsensoic_trasoniousData.ul;
  prevhtLevelta.ligcurrentDaht_level = sData.ligvioureidity;
  ptData.humrenity = curhumidiousData. prevng;
 Data.suhuRuare = currentom_temperatuousData.ro;
  previ.setTempurrentData cerature =et_tempviousData.sus;
  preta.acStatrentDa cur_status =sData.acreviouang;
  pata.jumlahOrcurrentD = ple_counta.peoousDat
  previ() {usDataioatePrevid upd);
}

voINTERVAL_UPDATE_INate >= MUpdPI- lastA(now return s();
  li now = miligned long{
  unsdate() ndUpcanSeool }

b
UT_UPDATE);_TIME_WITHO MAXdate >=ForceUpw - lastnorn (
  retu millis();long now =gned unsie() {
  ceUpdatldForbool shounge;
}

sChahareturn  }
  
  rue;
 ange = t hasCh= 10) {
   ) >ta.wifi_rssiusDaSI - previotData.wifiRS(abs(currenf 
  iference)Bm difange = 10 dgnificant chsinal change (iFi sig Weck Ch
  //
   true;
  }asChange =  hor) {
  nsltrasonic_se.ueviousDatar != prnicSensoultrasotData. (currenity)
  ifti proxime (mengganensor changnic seck ultraso/ Ch  }
  
  /true;
hange =     hasCHOLD) {
E_THRESANGT_CH >= LIGHight_level).lreviousDataevel - pata.lightLabs(currentDif (
  changeht level // Check lig
  }
  
  = true;ange     hasChESHOLD) {
NGE_THRMIDITY_CHA= HU >idity)ta.hum- previousDaidity ntData.humf (abs(curre i
 ty changeumidik hChec/ }
  
  /= true;
  hasChange OLD) {
    _THRESHANGEEMP_CH >= Tperature)oom_temata.rusD- previohuRuang rentData.sus(cur (abge
  ifrature chank room tempe// Chec
  
  rue;
  } t hasChange =
   Temp));a.setentDating(curr→ " + Str) + " peratureset_temiousData.tring(prev: " + S suhu ACrubahan Peintln("🔄Serial.pr {
    re)mperatu_teusData.setvio!= preetTemp rentData.s
  if (curgeanture chemperaCheck AC t}
  
  // = true;
  asChange 
    htatus);a.acSDat current → " + + ".ac_statussData previouAC: " +an status erubahn("🔄 Ptlrinerial.p  S) {
  c_statusata.a= previousD !tatusrentData.acS(cur
  if ngestatus chaheck AC 
  // C }
   = true;
    hasChange;
 ng))mlahOraa.jutDatring(currenSt" → " + count) + ata.people_ousDprevi+ String( " mlah orang:rubahan ju🔄 Pe"l.println(ria
    Se {unt)coData.people_ previous!=jumlahOrang rrentData. (cuORTANT
  if- MOST IMPt change coune heck peopl 
  // Ce;
 falssChange =  bool ha() {
 ntChangenificaSig=
bool has================CED NHANCTION - EDETE== CHANGE ======== =======
}

//  }
  };
  rol = falseiveContsAct.haControl;
      acde = falseualOverriontrol.man;
      acC= "auto"ode controlMtrol.     acCon
 otomatis");ali ke mode sting, kembi hoarif dntrol aktda kodak aln("✓ Ti.print Serialol) {
     eControl.hasActivntrif (acCo mode
    utofound, use aol ve contr No acti
    //else {
  } dBy);
    }ntrol.createCo" + ac"  Oleh: (tlnerial.prin    S");
   + "°C)emperature)trol.ac2Tg(acCon" + Strin) + " (" "MATI "HIDUP" :tatus ?ac2Strol.ing(acCon Str" +C2: "  A.println(  Serial
    °C)");ature) + "erc1Tempontrol.a+ String(acC + " (" I") : "MATUP""HIDatus ? ac1Stontrol.tring(acCAC1: " + Stln("  inl.priaSere);
      .controlModol acContrng: " + dari hostiC diterimaontrol Aintln("✓ Kerial.pr     S    
 
  ry
      }/ No expiesAt = 0; /ol.expircContr a       } else {

      faultminutes de; // 5  300000is() +millpiresAt = rol.ex  acCont      ) {
xpires_at"]ata["e  if (d    nt
esef prt is_arse expire     // Pa 
 e;
     ontrol = tru.hasActiveCControl      ac();
String>as<eated_by"]. = data["crdByol.createcContr a"];
     tureac2_temperae = data["emperatur2Tntrol.ac
      acCo];erature"ac1_tempdata["re = c1TemperatuacControl.a"];
      tatusa["ac2_s dats =ac2Staturol.Cont acs"];
     ac1_statudata["1Status = trol.ac
      acConerride"];l_ovta["manuaide = damanualOverr acControl.
     s<String>();].a_mode"control= data["Mode ol.controlontr    acCe
  statol  AC contrdate     // Up  
    a"];
 "dat data = doc[nObject     Jso
 ess"]) { doc["succrror &&  if (!e 
  e);
   c, responsalizeJson(doserirror = deError eization Deserial
   2> doc;cument<51icJsonDo{
    Statse)) , responGET", ""c_str(), "(url.stSRequeakeHTTP if (monse;
  
  String respATION;
 LOC DEVICE_on=" +atiD + "&loc" + DEVICE_Ievice_id=L) + "?dntrolUR String(acCong url =rirs
  Stery parameteth quRL wi// Build U  

  }
      return;D) {
NECTEWL_CONstatus() !=   if (WiFi.API() {
rolContid checkAC======
vo======= ====OL APINTRCOAC == ===== ==========

//lse;
  }
}fa
    return ttps.end();   hCode));
 nseg(httpResporinal: " + Stgag HTTPS rmintaan Peprintln("✗  Serial.
    } else { 201);
ode ==pResponseC|| httCode == 200 ponse(httpRes
    return ();nd    https.e();
nggetStrinse = https.po   res {
  0)sponseCode >httpRe
  
  if ();
  }= https.GET(Code Response   http else {
 oad);
  }ylPOST(pattps.ode = hnseC  httpRespo
  == 0) {T") "POSd, tho(strcmp(me
  if nseCode;t httpRespo 
  in");
 ESP32h", "ted-Wit"X-RequesaddHeader(
  https..0");Production/1martEnergy-SP32-S "Eent",er("User-Agadhttps.addHe");
  on/jsonatipplic", "a"Accept(eaders.addH");
  httpation/json, "applictent-Type""ConddHeader(https.a headers
    
  // Sete;
  }
als return f);
   "S gagalTP HT✗ Koneksi"ial.println(er {
    Snt, url))gin(cliebeps.
  if (!htt0
  1500om d fr // Reduce0000);  setTimeout(1 https.15000
 uced from 0);  // Redeout(1000t.setTimen cliER
  FAST timeout -
  // Sette);
  ACertificarootCt.setCACert(
  clientificatecert SSL  // Se;
  
 psient httPCl;
  HTTclientcure ientSe{
  WiFiCl& response) ringyload, St* past char method, con char*nstl, coar* urt(const chPSRequeseHTTool mak=======
b=======HELPER ===S REQUEST ====== HTTP===========// 

====");
}=============="======tln(erial.prin;
  }
  S "Failed"iStatus =entData.wif
    currGAL!"); WiFi GA"✗rintln(l.pia{
    Ser} else ;
  ()= WiFi.RSSIwifiRSSI currentData.   d";
 "ConnectefiStatus = ta.wientDacurr   main));
 ng(hostingDo Stri " +ain:om Drintln("✓al.priL));
    SerolURng(acContC: " + Strintrol A"✓ API Kotln(rial.prin
    SeRL));ng(apiU: " + Stri Produksi"✓ API.println(al;
    Seritring())ocalIP().toSWiFi.l: " + "✓ IPrintln( Serial.p
   NG!"); TERHUBU"✓ WiFil.println(Seria{
    NECTED) CON() == WL_WiFi.status  
  if (tln();
Serial.prin
  }
  tempts++;");
    atnt(".Serial.pri00);
    y(5 dela  
 s < 30) { && attemptCONNECTEDWL_) != tatus( (WiFi.shile= 0;
  wattempts 
  
  int ord); passw.begin(ssid,);
  WiFiWIFI_STAFi.mode(
  WiFI ===");N KE WIKAGHUBUNG=== MEN"ial.println() {
  SerWiFi(nect=
void con=========TION ======= CONNEC=== WIFI===== ========= }
}

//)");
 songan sudah ko(ruangi uk dikurangng untoraidak ada ntln("⚠️ Terial.prise {
    S} el
  g));ahOrang(juml + Strinotal: " - Tang keluarToAPI("OrdData sen  rData();
 Senso
    update WEBSITEG KEGSUNLANATE IRIM UPD
    // K");
    "/20(rial.println    Se;
mlahOrang)rint(ju.pal  Seri: ");
  m ruanganh orang dalaJumla"📊 int(ial.pr   Ser");
 NANGAUAR DARI RUANG KEL<< ORtln("🚶 <erial.prin  S  
    
lis();miled = nDetect  lastPersog--;
  umlahOran0) {
    jg > jumlahOran() {
  if (ExitrsonprocessPe

void 
}mlahOrang));tring(juotal: " + Smasuk - TPI("Orang taToAendDaData();
  spdateSensor  u
BSITE KE WESUNGUPDATE LANGIRIM   
  // Kux");
(" ltlnl.prin
  SeriaightLevel); tData.lint(curren  Serial.pr"); 
ya: "% | Cahaint(rial.pr; 
  Seidity)entData.humurr.print(c Serial; 
 umidity: ") | Hint("°C.pr 
  Serialuang);ntData.suhuRurreial.print(c; 
  SerSuhu: ")uk → ta saat mas"  📊 Daial.print(Serubahan
  at ada per sensor sadataTampilkan /   
  /");
"/20al.println(;
  Seriang)mlahOral.print(juriSe ");
  ngan:g dalam ruah oranmla"📊 Juint(erial.pr SGAN");
 UK KE RUANG MASRAN🚶 >>> Oprintln("l.eria();
  
  Smillisd = onDetectelastPers;
  g++ jumlahOranuk
  masrhasilg be 
  // Orann;
  }
 
    retur orang");al: 20itas maksimpasint("   Kapr  Serial.suk!");
  ak bisa maENUH - TidNGAN Pntln("🚫 RUA.priSerial) {
    = 20ang >jumlahOrif (
  as ruanganpasitek ka
  // CsonEntry() {ercessPid pro
vo}
, 20);
 0rang,ahOstrain(jumlrang = conlahO
  jumits/ Safety lim
  
  / break;
  } }
     DLE;
     tate = IurrentS    c {
    ME + 200))CTION_TIIN_DETE> (MtartTime  stateSnow -  if (E
    i ke IDLalsebelum kembtar benunggu se     // TCOMPLETE:
 CE_SEQUEN 
    case reak;
         b
     }LE;
   State = ID    currentME) {
    CTION_TI> MAX_DETErtTime  - stateStae if (now
      } els;E_COMPLETEENCte = SEQUentSta    currewati
    esai melg sel - oranaktifdak a sensor tiKedu      // d) {
  tOutDetecte.objecata!ultrasonicDected && nDetctIjenicData.obraso if (!ultD:
     H_TRIGGERE    case BOT  
    reak;
   b     }
   = IDLE;
 ntState       curre  E) {
ECTION_TIM > MAX_DETTimetartstateS (now -  else if     }e = IDLE;
 at   currentSt     }
     );
   t(xiersonEessPproc
          NG KELUARORA KOSONG = DUA →KE: KELUAR →  // Sequence
         ME) {ETECTION_TI MIN_DtartTime >=now - stateS if (     i
  ce selesasequenak aktif -  sensor tidKedua      // {
  ed) etect.objectOutDcDatasonid && !ultratecteDeata.objectInsonicD if (!ultrase} el   GERED;
   _TRIGTHState = BOnt       curremelewati
 rang sedang r aktif - oedua senso/ K
        /ted) {ctOutDetecnicData.objerasoted && ultctInDetecjeta.obicDaif (ultrason
      IGGERED:_TRe OUT cas  
   
         break;  }
     ;
State = IDLE  currentE) {
      TIMN_ECTIO_DETAX MTime >artstateSt- se if (now    } el IDLE;
   tState =curren}
            
    ();trysonEncessPer      proASUK
     = ORANG MUA → KOSONGUK → KEDnce: MASSeque   // 
       ON_TIME) {CTIMIN_DETE >= tartTime stateS if (now -      e selesai
  sequencif - tidak aktnsordua se      // Ke  d) {
cteteDeobjectOuticData.ultrason && !tedtec.objectInDetaultrasonicDaelse if (!D;
      } H_TRIGGERE= BOTentState       curr
  elewating mng sedaktif - oraor a sensuaed       // K
 ) {ectedobjectOutDetata. ultrasonicDected &&ctInDetjeobsonicData. if (ultraRED:
      IN_TRIGGE 
    case
     break;}
      ;
      owtartTime = n    stateS
    TRIGGERED;ate = OUT_rrentSt    cu    {
 utDetected).objectODataic && ultrasonedInDetectta.objecticDa (!ultrasone if     } else = now;
 tTimteStar      staRED;
  IN_TRIGGEe = tat   currentS    
 ected) {DettOuta.objecnicDataso!ultrDetected && bjectInata.ocDasoniif (ultr  uluan
     dtif yang akanaek sensor m idle, cri    // Da:
  IDLEe cas    te) {
rentStaswitch (currgerakan
  ksi arah pentuk detee machine u Stat  
  //
  }
bug = now;    lastDe);
at ramai)"ln(" (Sangl.print  else Seria);
  p ramai)"(" (Cukurial.printlng <= 15) SejumlahOranif (else g)");
     oran" (Sedikitprintln(Serial.g <= 5) hOranlaumse if (jel    song)");
(Ruangan kotln(" in Serial.prrang == 0)jumlahO    if (20");
rint("/ Serial.p
   rang);ahOmlprint(ju    Serial.");
ngan: ruag dalam ran otu - Jumlahatus Pin"📊 Stial.print(
    Ser10000) {stDebug > - lanow  (0;
  if = g lastDebugned lontatic unsigik
  setp 10 dtiaseederhana s slkan statuampi  
  // Turn;
  }
   ret
 LDOWN) {COOd < PERSON_nDetectetPerso - lasif (nowsi ganda
  ah detektuk menceg Cooldown un
  //s();
  = millig now  lonsigned() {
  unaUltrasonicd bac======
voi==========MITY) =ANTI PROXILOGIC (MENGGION EOPLE DETECT======= P======// ====
}

tected;OutDeta.objecticDaasontrd || ulectInDetectea.objrasonicDat ultnsor =sonicSe.ultraData currentk API)
  (untustatusent sensor rrte cu/ UpdaN);
  
  /_DETECTIOceOut <= MAXanistData.dsonictra          ul                      && 
      CE MIN_DISTAN>=istanceOut asonicData.dltred = (uutDetecta.objectOsonicDat
  ultraON);CTI MAX_DETEtanceIn <=disonicData.  ultras                        
          NCE && TAIN_DISeIn >= Mta.distancrasonicDaected = (ulteta.objectInDattrasonicDule 1-10cm
  am rangk dalobjeeksi 
  // Det + 1;
  DISTANCEceOut = MAX_distanta.ltrasonicDaOut == 0) ustanceData.dionictras(ulif CE + 1;
  X_DISTAN= MAeIn istancata.dultrasonicDn == 0) stanceIonicData.dirasult
  if (atau error)nge  of ra(0 = out pembacaan  // Validasi
 );
  IN_OUT_PCHO EPIN_OUT,e(TRIGGER_stancnicDireadUltrasoOut = nceta.distacDanii
  ultrasoferensri internda menghior untukens sarntDelay alay(50); // 
  deO_PIN_IN);_IN, ECH_PINRIGGERDistance(TicasoneadUltr rdistanceIn =icData.son ultraua sensor
 kedk dari // Baca jara
  cSensors() {ltrasoniid readUce;
}

voistan return d}
  
 rn 0;
   {
    retuCE) MAX_DISTANstance >e <= 0 || dif (distancr
  ie atau erroof rangika out eturn 0 j  // R
034 / 2;
  on * 0.= duratie ancst  long di2
 0.034) / ation *k = (durRumus: jaracm
  // alam ak djarg  // Hitunms
  
  30meout// TiGH, 30000); in, HIseIn(echoPuluration = p d longecho
 Baca pulse 
  
  // );gerPin, LOWe(trigitalWrit  dig0);
(1dsayMicrosecon;
  delHIGH)triggerPin, igitalWrite(s(2);
  dcondyMicrose;
  delain, LOW)riggerPWrite(titaligger
  dtrigulse rim p // KiPin) {
  echoggerPin, intint tricDistance(dUltrasoni
long rea================OXIMITY) =NGGANTI PRIONS (MEUNCTLTRASONIC F= U==========/ ======

/eck = 0;trolChastACCong lgned lon= 0;
unsidate Upceg lastForgned lon
unsipdate = 0;ong lastAPIUunsigned les
bliming varia;

// TatarrentD} cuI = 0;
ifiRSS";
  int wcted"Disconnes =  wifiStatu;
  Stringestamp = 0ong timned lunsigor
  Sensityoximganti prenge;  // M= falssor rasonicSenool ult;
  b= 0lightLevel  int  0.0;
 t humidity =  floaang = 0.0;
t suhuRu 0;
  float setTemp = in "OFF";
 Status =ng ac
  Stri0;g = t jumlahOran {
  insorDatat Sen
strucata d sensor Currentata;

//eviousD0;
} prssi = i_r  int wify_sensor
nti proximit // Menggafalse; _sensor = ultrasonic bool l = -1;
 velent light_;
  i999.0midity = -hu
  float  -999.0;rature =_tempe  float roome = -1;
aturmper  int set_teus = "";
ng ac_statriStt = -1;
  people_coun  int 
sData {uct Previouion
stretecthange des for csor valus sen
// Previou = -1;
astTemp2-1;
int llastTemp1 = t = false;
in lastAC2 lse;
bool = fastAC1latate
bool re Sardwa;

// AC HacControl;
} lsentrol = faActiveCoasool h";
  batedBy = "ring creSt
   = 0;Atxpiresg egned lon5;
  unsi 2emperature =nt ac2T= 25;
  irature Tempe ac1
  intfalse; = ol ac2Status  bo= false;
 ac1Status boolfalse;
   = rride manualOve boolschedule
 anual, o, m// aut;         "auto" =deMo controlringte {
  StolStatruct ACCon State
strntrolC Cota;

// AtrasonicDa ul false;
}Detected = objectOutool b
 ;= falseed etectl objectInD
  booeOut = 0;ncint dista
  anceIn = 0;dist
  int  {asonicDatact Ultr
struity data)imganti proxdata (meng sensor sonictra

// Uled = 0;DetectlastPersoned long 
unsignrtTime = 0; stateStagned long
unsitate = IDLE; currentSctionStateDetesai
};

e seleuenc // SeqOMPLETE_CENCEEQUf
  S aktia sensor// KeduRIGGERED, OTH_Tr aktif
  Br keluanso // Se, REDUT_TRIGGEf
  O masuk aktiorD,   // SensN_TRIGGEREi
  Iksnunggu dete      // MeDLE,     ate {
  ISttection
enum Dey logic) proximitenggantisi arah (mtekine untuk deState mach -1;

// lahOrang =astJum0;
int lOrang = nt jumlah
i=================VARIABLES ===== ===========// =

eteksitar d andetikldown 2      // Coo000 WN 2N_COOLDOERSO
#define Psequencedetik untuk  Maksimal 3 ME 3000   //TECTION_TIe MAX_DE
#defindi valituk detekss unMinimal 300m0    // N_TIME 30CTIO MIN_DETEine0 cm)
#defimal (1-1pteksi oange det     // RION 10ETECTe MAX_Din
#defcm)deteksi (1 rak minimum  // JaE 1      _DISTANCine MINcm)
#defr (400 um sensoarak maksim J//     TANCE 400AX_DIS=
#define M============== ==ERSION PARAMETIC DETECT== ULTRASON========/ =======

/;DHTTYPE)DHTPIN, dht(

DHT    34    PIN#define LDR_T22
E       DHfine DHTTYP#de      27
N  ine DHTPI====
#def=======INNYA ======SOR LA===== SEN============R

// ELUAsor K echo sen Pin 17   //UT   _PIN_Oine ECHO
#defAR KELUensortrigger s// Pin 16   OUT GGER_PIN_#define TRIor MASUK
n echo sens   // Pi   15O_PIN_IN  e ECHR)
#definENSOnti PROX_SSUK (menggansor MAn trigger se // PiPIN_IN  2    TRIGGER_==
#define===============OXIMITY) GGANTI PRIC (MENTRASONSENSOR UL==== ========
// =====PIN);
ac2(IR_asonicAc );
IRPan1(IR_PINonicAc ac4
IRPanasfine IR_PIN c.h>

#deasoniPane <ir_ludd.h>
#incIRsene <ludh>
#incoteESP8266.<IRreme #includ
======================= IR ========== =
// Tinggi"
ik TeganganLab TeknATION "CE_LOC#define DEVIoduction"
rt_Energy_PrmaID "ESP32_Sfine DEVICE_deon
#atiInformDevice / ng)

/ebih seri 5 detik (ltiap secontrol// Check AC 00 VAL 50CHECK_INTERONTROL_ne AC_Ceficepat)
#dh  (lebiateupddetik antar / Minimum 1   / 1000    VALINTER MIN_UPDATE_
#defineing)ih ser(lebit iap 2 mensa setm pak0 // Kiri0020T_UPDATE 1THOU MAX_TIME_WIfineif)
#delebih sensit0 lux (berubah >= 3ika cahaya m j    // Kiri0  3_THRESHOLDHANGEHT_Cfine LIG
#densitif)bih se5% (le>= 1.ubah er bdityumi hka/ Kirim ji.5 /ESHOLD 1CHANGE_THRTY_IDIine HUMitif)
#def(lebih sens 0.3°C berubah >=hu a su Kirim jik   //D 0.3 _THRESHOLEMP_CHANGEdefine TES
#STER UPDATuration - FAction Configange Dete";

// Ch-\n----CERTIFICATEEND -----
" \GCc=\n"os1AnX5iItre1/+jh5XQ99b2Jm+kXs+lVDR3vey/KJ4EBgcYxn/eR44yPx
"em" \d\nxA57Ezw1SgEXq37Z0j4r7gbhPgXRR4gc/usnvEiU0ZvgJBQl9nJcnWE
"mRGunUHBn" \9d11TPA\E44Za4rF2LN0G9yiy0Ff/9U/t7dd8XM2w7jXbyLeiNTH9w4RgqsahDYVv\n" \
"4kqKOJ2qxqlvh+wjChPUi0/1fpsMljq4s5ZubQVG1CceWxjTOQ5yOf+1i3B43nn" \
"oyb+ZAJzVc\R1NbdWhscdC95VK7q4+63SM1NFKxHpHzsTDk4JUagEiupzVmbUqjNPEl\n" \
"dCNwW0wdeRlN8Nxe5Ad7u+KAIuvtl+/+sLF+3JjFaH3pOhqpZXmzOVyygStATkXW"\
" Ur\nhdjsnvzqvHu7LCgdNbOwTdwJx4nztXOoJm57iab4phowiJ+GslWYHRAzI4JMP \
"OKQ5\n"NzktHCgIInfHTjuCvjHv3xLLLW6WVwC3+6DyiUwthyVS5uCbMiogz
"NFtY2PwBn" \K\cOj/KsRqkSsimvoiBOv/2X//GpsMiu/X1+o77x/Py9auJtF8GaV0nxvwubYhBe
"3\n" \GQ+GnY+EgPbk6ZnrzuBZ6brJFeWhfFQDlyFAvqF7LSYK05qFnTIZd+50xx+"ubhzE \
V9lZL\n"LlaS/u+IgJykIrGF1XIpmkDLHYGDQYqbyyqFAOCAgEAVR9BAQsFAG9w0"hki\
\n" qbjANBgkIyBpY9umb5AFzgAiBBR5tFnme7bldDgQW0GA1UBTADAQH/MB
"HRMBAf8E" \nBBjAPBgNV\wEB/wQEAwI4GA1UdDjBAMABAAGjQPAgM7/vGT1M0Ns6xt0nUW"rU7m2Y
 \"53CI\n4YC1CLQJ13hefo//fb4hVOPNk3sgrDQo8faqU9GY/rdrO0aoSCqI3Haa" \
"qHyGbw\nWBfEbwrQbooMDQaHymCzLq9gwZ9YIqioxIt4I7mKsEwMFrQHu8BCNAw1FtxNjh\n" \
"3SzynTn3xDkuBQES3MFEvzG6vBbI+e0ocUXgVCwOPjdA0+gV4WehjuefXKnE\
"OlFuhmbWn\n" H0ao0xAJEadaT5ZPt0m9STQZBKFleaJ1/ctaJxrOrHN70pjBqAlkd25
"KBdsn" \b0SHzUv\f1BdpUKD9j0KtZB6aw/lJBdiB3QWHZu8A77DnKx2dZ3lg0giV5+I\
"B5iPNC\n" j3x+Uvu396AQ4Q7e2RCOFmxdi85tukm87kGJ5B4YlcNHlsdu5i5T0Y3HsLuJvW"B \
2FR3qyH\n"Jom/ELNKjD+VMk7BPZ7hIlDHCNAymg4ye0rgTB/7V3LvST8KOEUt+zwvo \
"q+sW\n"3dpj18jA8+o+uRtwNwIttov0DiNe4Mu0UlXjIB0g4rk8KbgjU+"A5/TR5d8mU" \
iS4+3mX6U\nz5MDmgK7Pfjxwv60pIgbBioZxaUpmZmykv/EvEhmfs7j3Xn"0TM8ukj1\
" yF+\nuoxmSa78f0ESTtrFj/RQ3miGbvVqbvYATyjbZXoHj3dcKi/ct984kIxuPO"h77\n" \
fzm54rVygcoJHP0FDK3CggIBAgIPADCCAgoQEBBQADgIhvcNAwDQYJKoZMTCCAiI\
"9vdCBY\n" ElTUkcgUmNVBAMTDATBgAxFTvdXggR3JzZWFyY2kgUmVpdHgU2VjdXJ" \
"ZXQXJu\n0ZMgSW5ECh1UMCcGAVQQGEwJVUzEpswCQYDwNDM4WjBPMQNMzUwNjA0MTE
"WhcM4\n" \wNDMTEHhcNMTUwNjA0FJvb3QgWDEwEwxJU1JHIYDVQQDEw3VwMRUw"cmNoIEdyb\n" \
c2VhVyaXR5IFJlmV0IFNlY3yboTIEludGVBATAnBgNVxKUEBhMCVVMGA1
"TzELMAk" \QAw\nhvcNAQELBwAwDQYJKoZICiZRGPgu2OSQONBAgIRAIIQz7DAwIMIIFazCCA1Og\
"-\n" ATE----FICRTIN CE
"-----BEGIte = \ACertificaar* rootC
const chioniguratnfSL Co

// Sol";/ac/contr.ac.id/api.fst.unjattps://dasko = "hlURLcContro ahar*const cta";
r/da/api/senso.ac.idko.fst.unja/dashttps:/piURL = "onst char* a
cd";t.unja.ac.idasko.fs"ngDomain = tichar* hosion
const ratng Configuon Hosti
// Producti2026";
 = "LABTTTsswordt char* paons
cTINGGI";NGAN K TEGAEKNI Td = "LABt char* ssiconsuration
 WiFi Config=======
//========= =NFIGURATION===== CO==========t;

// == tf
TFT_eSPI