USES
  CRT,
  SB,GUS,Songs,ModLoad, cucugbas, pcx, graph,dos;

const
     YEncVelas = 105;
type tletra = record
            p : pointer;
            x : integer;
       end;
VAR
  Song:TSong;
  letras : array [1..16] of tletra;
  Speaker : boolean;



  PROCEDURE SetMaxQuality;
  BEGIN
    SoundMode.StMix:=TRUE;
    IF DSPVerMajor<4 THEN
    BEGIN
      IF (DSPVerMajor=2) AND (DSPVerMinor>0) THEN SoundMode.Freq:=44100
      ELSE SoundMode.Freq:=22050;
      IF DSPVerMajor=3 THEN SoundMode.Stereo:=TRUE
      ELSE SoundMode.Stereo:=FALSE;
      SoundMode.b16Bits:=FALSE;
    END
    ELSE BEGIN {SB16 o superior}
      SoundMode.Freq:={44100}22050;
      SoundMode.Stereo:=TRUE;
      SoundMode.b16Bits:=TRUE;
    END;
  END;

  PROCEDURE InitSoundMode;
  var cadaux : string;
  BEGIN
    cadaux := paramstr (1);
    speaker := upcase (cadaux [1]) = 'S';
    IF GUSDetect And not speaker THEN
    BEGIN
      SoundMode.Device:=devGUS;
      GusInitValues;
    END
    ELSE IF SBDetect and not speaker THEN
    BEGIN
      SoundMode.Device:=devSB;
      SetMaxQuality;
    END
    ELSE BEGIN
      SoundMode.Device:=devSpeaker;
      SoundMode.Freq:=18000;
      SoundMode.Stereo:=FALSE;
      SoundMode.b16Bits:=FALSE;
    END;
  END;
  FUNCTION TeclaPulsada:Boolean;
  BEGIN
    TeclaPulsada:= MemW[$40:$1A]<>MemW[$40:$1C];
  END;

Procedure EncVelas;
Var
    x,cont,llama     : integer;
   col    : array [0..5] of integer;


Begin
     setrgbpalette (240,63,63,63);
     for X:= 1 to 160 do
     begin
          for cont := 0 to 3 do
              col [cont] := getpixel (x + cont, YEncVelas);
          for cont := 0 to 3 do
              putpixel (x + cont, YEncVelas,14);
          delay (4);
          for cont := 0 to 3 do
              putpixel (x + cont, YEncVelas,col [cont]);
          case x of
          140 : setrgbpalette (240, 63,20,0);
          160 : setrgbpalette (243, 63,63,0);
          end;
    end;
  llama := 0;
setrgbpalette (240, 63,20,0);
setrgbpalette (243, 63,63,0);
{  for cont := 1 to 300 do}
repeat
          if (llama mod 8) = 0 then
          begin
               setrgbpalette (241, 63,63,0);
               setrgbpalette (242, 63,40,0);
          end;
          if (llama mod 8) = 4 then
          begin
               setrgbpalette (241, 63,40,0);
               setrgbpalette (242, 63,63,0);
          end;
          if (llama mod 12) = 0 then
          begin
               setrgbpalette (244, 63,63,0);
               setrgbpalette (245, 0,0,0);
          end;
          if (llama mod 12) = 6 then
          begin
               setrgbpalette (244, 0,0,0);
               setrgbpalette (245, 63,63,0);
          end;
          llama := llama + 1;
          delay (8);
    until (song.notepos >= 50);
End;


Procedure ApagarVelas;
var
   c : integer;
Begin
     SetRGBPalette (244,0,0,0);
     SetRGBPalette (245,0,0,0);
     SetRGBPalette (240,40,40,40);
     setfillstyle (solidfill,0);

     bar (130,90, 190,108);
     setcolor (240);
     setlinestyle (0,0,2);
     line (148, 103,148,108);
     line (149, 103,149,108);
     line (170, 103,170,108);
     line (171, 103,171,108);
     setlinestyle (0,0,1);

     for c := 1 to 40 do
          SetRGBPalette (242,40+random (20),10+random (10),0);
     SetRGBPalette (242,20,20,20);
End;

var
  cont      : integer;

Procedure CargarLetras;
var
   nl : integer;

Procedure CL (let : char; x : integer);
var f : file;
    p2 : integer;
Begin
     assign (f, let + '.pun');
     reset (f,1);
     getmem (letras [nl].p, 256);
     blockread (f,letras [nl].p^,256, p2);
     close (f);
     letras [nl].x := x;
     inc (nl);
End;
Begin
     nl := 1;
     CL ('f',100);
     CL ('e',118);
     CL ('l',136);
     CL ('i',154);
     CL ('z',172);
     CL ('c',208);
     CL ('u',226);
     CL ('m',244);
     CL ('p',262);
     CL ('l',280);
     CL ('e',298);
End;

procedure CaerLetras;
var c,i : integer;

Begin
     setcolor (0);
     for i := 1 to 11 do
     begin
          for c := 1 to 40 do
          begin
               delay (6);
               putimage (letras [i].x, c,letras[i].p^,0);
               line (letras [i].x, c-20, letras [i].x + 16, c-20);
          end;
          for c := 20 to 40 do
          begin
               line (letras [i].x, c, letras [i].x + 16, c);
               delay (4);
         end;
      end;
End;

Procedure EfectoPant;
var d :integer;
begin
     EsperarBarrido;
     setrgbpalette (250,63,0,0);
     setrgbpalette (0,63,0,0);
     Delay (500);
     d := 63;
     repeat
         setrgbpalette (0,d,0,0);
         dec (d);
         delay (25);
     until d < 20;
     setrgbpalette (0,0,0,0);
     ponerpcx ('a.pcx',8,153);
     setrgbpalette (251,0,0,0);
     d := 63;
     EsperarBarrido;
     setrgbpalette (251,0,63,0);
     setrgbpalette (0,0,63,0);
     Delay (500);
     repeat
         setrgbpalette (0,0,d,0);
         dec (d);
         delay (25);
     until d < 20;
     setrgbpalette (0,0,0,0);
end;

procedure EscribirCucu;
Var
   C1, c2,aux : integer;
Begin
         ponerpcx ('c.pcx',220,130);
         for c2 := 201 to 230 do
         begin;
             setrgbpalette (c2,0,10,40);
             setrgbpalette (c2-1,0,20,50);
             setrgbpalette (c2-2,0,30,63);
             delay (40);
         end;
         for c2 := 229 to 239 do
         begin;
             setrgbpalette (c2,40,0,0);
             if c2 > 229 then setrgbpalette (c2-1,50,0,0);
             if c2 > 230 then setrgbpalette (c2-2,63,0,0);
             delay (20);
         end;

End;

Procedure ENieve;
Type TCopo = record
          x, y,ant:integer;
          dib, atras : boolean;
     end;
Var
   c3 : integer;
   pal  : array [1..5000] of RGBColor;
   aux : RGBColor;
   copos : array [1..200] of tcopo;
   cont,c2 : integer;
   BastaNieve, Termino : boolean;
Begin
     setrgbpalette (100,63,63,63);
     For cont := 1 to 200 do
     begin
          copos [cont].x := random (400) - 80;
          copos [cont].y := random (200) - 200;
          copos [cont].dib := false;
          copos [cont].atras := random (2) = 0;
     end;
     aux.r := 0;
     aux.g := 0;
     aux.b := 0;
     fillchar (pal, 3 * 50, 0);
     randomize;
     setpartvgapalette (110,40, pal);
     for cont := 0 to 37 do
     begin
          setrgbpalette (cont+110, 0,0,0);
          for c2 := 0 to 319 do
              for c3 := 0 to 5 do
               if getpixel (c2, cont * 5+c3) = 0 then putpixel (c2,cont*5+c3,cont+110);
     end;
     For Cont := 2 to 5000 do
     begin
          pal [cont] := pal [cont-1];
          if (aux.r = pal [cont].r) and (aux.g = pal [cont].g) and (aux.b = pal [cont].b) then
          begin
               aux.r := random (60)+4;
               aux.g := random (60)+4;
               aux.b := random (60)+4;
          end;
          if pal [cont].r < aux.r then inc (pal [cont].r);
          if pal [cont].r > aux.r then dec (pal [cont].r);
          if pal [cont].g < aux.g then inc (pal [cont].g);
          if pal [cont].g > aux.g then dec (pal [cont].g);
          if pal [cont].b < aux.b then inc (pal [cont].b);
          if pal [cont].b > aux.b then dec (pal [cont].b);
     end;

     BastaNieve := false;
      c2 := 1;
     setcolor (15);
     settextstyle (smallfont,HorizDIr, 1);
     outtextxy (40, 190, 'Pulsa una tecla para terminar...');
     repeat
           termino := true;
           For cont := 1 to 200 do
           with copos [cont] do
           begin
                if dib then putpixel (x,y,ant);
                if (random (5) = 0) then inc (x);
                inc (y);
                ant := getpixel (x,y);
                if (y > 0) and (y < 190) and
                   ((getpixel (x,y) <> 100) and
                      ((getpixel (x,y) in [0,110..139]) or not atras))
                then
                begin
                     putpixel (x,y,100);
                     dib := true;
                end
                else dib := false;
                if (y >= 190) And Not BastaNieve then
                begin
                     y:= random (50) - 50;
                     x := random (400) - 80;
                end;
                if (y < 190) then termino := false;
           end;
           if (TeclaPulsada) then bastanieve := true;
           setpartvgapalette (110,40, pal [c2]);
           inc (c2);
           if c2 > 4950 then c2 := 0;

     until Termino;
End;

BEGIN
  InitSoundMode;
  SongInit (Song);
  LoadMod ('breakfre.mod',Song);
  IF MLError<>OK THEN
  BEGIN
    WriteLn(MErrStr(MLError));
    Halt(2);
  END;
  CargarLetras;
  empezargraf256;
  scroff;
  PonerPcx ('p.pcx',0,0);
  for cont := 200 to 255 do
      setrgbpalette (cont,0,0,0);
  setrgbpalette (101,0,0,0);
  scron;
  SongPlay(Song);
  EncVelas;
  ApagarVelas;
  ponerpcx ('e.pcx',10,32);
  Delay (2000);
  CaerLetras;
  EfectoPant;
  EscribirCucu;
  Enieve;
  closegraph;
  SongStop(Song);
  SongDone(Song);
END.