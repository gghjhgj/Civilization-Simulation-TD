~~wyniki optymalizacji:

milion: 150+-
milion: 150-155 bardzo podobnie
milion: 200 bo rzadziej radius
milion: 230+- rzadziej radius i zero rand
milion: 250-260 to co wyżej + isvalid wywalone
milion: 280-300 getcellref wywalone i chunkref uproszczony
milion: 400+- czasem 360 czasem 440 to co wcześniej ale co 3 ruchy sprawdz na jakim chunku jesteś i czy jest na nim to czego szukasz 
milion: 450-500 - wywalenie moves z ifa i radius searcha na petle po wszystkich ludzikach jeśli humanticks wynoszą x
milion: 500-530 - wywalenie moves ze struktury ludka i ich wszystkich zaleznosci
milion:700-750 - wywlenie getcell water na humanmove
milion: 1100+- ludki robia kilk ruchow na ticku

* Od czego najlepiej byloby zaczac?
    - wydaje mi sie ze ogarniecie rendera nawet nie idealnie ale zeby dawal w miare rade, pozniej wprowadzenie obslugi swipowania i zmiana wielkosci swiata na np 60k x 60k i zobaczenie co sie stanie, po 1 z ludzmi a po drugie z tym ile wyzera swipe sam w sobie jak duzo fajnie by bylo to zoptymalizowac, ale dopiero wtedy, na poczatku ma dzialac
    - jak wystapi paraliz najlepsza decyzja bedzie zaczecie od bawienia sie decyzjami cywilizacji i patrzeniem co sie dzieje i czy decyzje sa lepsze czy to stworzenie jest jeszcze glupsze niz bylo

    
~~Co jest:

1) World:
    - struktura swiata chunk 32bitowy 3x3 pol po 3 bity + flagi
    - chunkRegion - 4x4chunki 
    - swiat na caly ekran - 1.8mln pol i pikseli

2) Cywilizacja:
    - prosty decission making, budowanie domow, przydzielanie pracy ludziom, zasoby

3) Ludzie:
    -podzial na role
    -radius search
    -pathfinding

4) Multithread i podzial rol:
    - petla symulacji + render leci po rdzeniu 0 (2 watki)
    - petla ludkow kreci sie asynchronicznie do symulacji po rdzeniu 1-5 (10 watkow)
    - mamy faze synchronizacji zeby nie robic race condition np na cywilizacji czy renderze bo wywalalo segfaulta
    - ludki kreca kilka ruchow jako wszystkie ludki zrobcie tyle ruchow (petla na zewnatrz nie wewnatrz, moze lepiej wewnatrz?)
    - builders i assigned maja dodatkowy sync w srodku bo modyfikuja rzeczy krytyczne
    - zbieracze nie maja tego syncu w srodku, modyfikuja mape na biezaco, ub nie zauwazylem, segfaulta tez nie dostaje
    - czasem wychodzi ze ludki zebraly surowiec w jednym momencie (np delikatnie ujemna ilosc kamieni gdy ich nie ma na mapie) ale nie jest to rzecz krytyczna i nie wiem czy chcemy wprowadzac takie sprawdzanie kosztem wydajnosci bo surowcow zbiera sie duzo, byloby o (n log n) dodatkowe i pytanie czy to potrzebne

~~Co nie dziala:
(to wszytko nie sa bledy krytyczne nie trzeba sie nimi zajmowac koniecznie od razu, czesc z nich moze sama zniknac)
1) czasem cos sie zespawnuje na budynku ktos to nadpisze i sa dziury, bardzo rzadko ale sie zdarza
2) ludki lubia sie zablokowac - chodzic tam i spowrotem w jakies strukturze robiac pociagi i zle to wyglada i psuje wrazenia wizualne
3) render na 100mln ludzi cos odwala (renderuje co 2 tick swiat bez ludkow)- moze byc to kwestia tego ze trzeba go jak czlowiek napisac po gpu 
4) nie wiem jak to nie wywala programu ze niektore rzeczy modyfikuje i z petli symulacji i z petli ludzi, moze dobry timing, narazie tego bym nie ruszal bo nie mam pomyslu jak to rozwiazac madrze tak zeby zachowac izolacje miedzy ludkami a symulacja i nie kazac im na siebie czekac


~~ToDo symulacja

1) Render:
    - wprowadzic render w pelni po gpu, najlepiej wydzielic to tak zeby potem dalo sie zrobic migracje na vulkana, nie chcemy robic tam jakichs dziwnych rzeczy tylko wez to i narysuj
    - wprowadzic ruch kamery i jakos ladnie to zoptymalizowac
    - moze (nie konieczne) przyblizanie 
    - bardzo moze oddalenie i zmniejszanie lod to, wymaga wiekszej architektury, pytanie czy to konieczne

3) World:
    - zwiekszyc rozmiar swiata na dobry start do 4mld pol pozniej potencjalnie zmienic wielkosc danych np x y ludka x y pola czy chunka etc z uinta16_t na uint32_t i walnac mozna jakis wiekszy, pytanie czy to tego warte bo 4mld pol to okolo 1.8gb w ramie wiec sie zmiesci potencjalnie moze bym zmiescil 10-20mld a wiecej to znowu robienie architektury nowej pipelinu danych z ssd na ram streamowanie aktywnych regionow i duzo roboty a nie wiem czy jakis taki wielki swiat typu 1bln pol jest potrzebny a jakby sie ludzie rozproszyli to i wydajnosc moglaby spasc i duzo ale jest za tym

4) Ludzie:
    - napewno trzeba jeszcze przysiasc nad optymalizacja, najbardziej oczywiste zyski sa zrobione ale imo da sie to rozbic jeszcze mocniej i moze nawet tam duza czesc hitnac simdem, duzo roboty ale jest tam do wycisniecia jeszcze troche
    - napewno nie podoba nam sie ich radius search i szukanie w swoim chunku bo te ludki sa bardzo glupie i tez przez to sie grupuja ale z drugiej strony to zalezy od gestosci surowcow bo jak mala to wtedy fajnie by weszla logika jak z szukaniem chunkow czyli bierszesz caly region ale szukasz rzadziej ale jak gesto to nie chcemy skanowac czesciej takiego obszaru, moze zmneijszenie regionow jest pomyslem i wtedy skan na region ale czestszy? ale przy ogromnym swiecie cache bedzie cierpiec prawdopodobnie a dodawanie dodatkowych jakichkolwiek mechanik bardzo mocno obnizy wydajnosc napewno
    - walnac template juz po wszystkim na ta funkcje zeby nie zajmowalo to 400linijek

5) Cywilizacja:
    - logika: to jest tak glupie stworzenie ze napewno tam trzeba zmienic przydzielanie ludzi do pracy, budowanie, no ogolnie poprostu ogarnac to
    - sama cywilizacja jest bardzo plaska i ma bardzo malo mechanik, fajnie byloby dodac np pitos w koncu zrobic cos z tym food ale nie per human tylko per cywilizacja i np morale? cos takiego

    * tutaj chodzi o znalezienie celu tego po co te obliczenia i to wszystko na ekranie sie dzieje
    - sa te armie i potwory juz napisane i dobre wyniki potrafia wykrecac, bardzo beta wersja ale ma potencjal, moznaby tego uzyc
    
    - jest opcja z wzieciem tych armii i potworow i zrobieniem z tego td poprostu

    - co by moglo byc tez ciekawe to dodanie kilku cywilizacji ciagniecie tego jakos ladnie moze tam dodanie im jeszcze 1 rdzenia od ludzi(w ostatecznosci), wtedy moznaby wprowadzic potencjalnie to na zasadzie sa cywilizacje rozwijaja sie kazda ma swoja pule ludzi etc, nie chcialbym robic tego osobno, szczegolnie ze jak cos mozna walnac ticksy na 1 petle ludkow np x ilosc cywilizacji i dawanie lambd dodatkowych etc nie zaboli az tak (chyba)
    - wtedy logika armii wchodzi spowrotem tylko w lepszej formie i ta cywilizacja co lepsza armie ma i lepsze zasoby etc ta wygrywa zdobywa teren jakies wplywy, no nie wiem cos tego typu 

    - moze jakies poprostu a'la rimworld? czy factorio ze czysta ekonomia

    * nie wiemy co tu bedzie uzyte ale trzeb to wymyslec

6) stricte clean code:
    * mobilnosc programu, nie hardcodowanie watkow czy rdzeni i logika tego ile przydzielamy i kiedy etc
    - wywalenie rzeczy nieuzywanych np z configa czy struktur danych

    * podzielenie plikow ladnie na moduly itd, proste ale duzo jebania z includami bedzie 
    - poinlinowanie funkcji czy pododawanie const gdzie trzeba, powydzielanie roznych rzeczy poza np funkcje czy osobne pliki miejace takie rzeczy, np lookup table etc, czy np odhudzanie klas poprostu i odpowiedzialnosci, dodanie troszke abstrakcji
    - wywalenie dziwnych komentarzy typu //git //nie git co pozostawaly po refaktorach

    * dodatkowe rzeczy
        - cmake
        - plik konfiguracyjny z configami tam moze byc troszke wiecej roboty zakladam i nie wiem czy to wogole przechodzi na c++ dobrze 
        - testy jednostkowe fajnie byloby zrobic albo jakis tryb debug podglad danych jawnie itd 

7) features:
    - okienko ze statystykami zamiast txt 
    - wstawianie np wymiarow swiata ilosci ludzi etc moze z poziomu generatora swiata nie pliku konfiguracyjnego i ustawienia poprostu - to mozna hitnac obsluga myszki i klawiatury sfml i jakas banalna grafika poprostu


8) a co dalej?
    * jak to wszystko co wyzej sie skonczy to powrot do silniku vulcana i tu projekty co fajnie byloby zrobic:
        - silnik do animacji 3d i 2d jakis taki mobilny 
        - jakis fajny compute moze symulator czasteczek 

    * jak to pojdzie fajnie byloby przelaczyc to co w sfml jest a mogloby byc w vulkanie na vulkana wlasnie i zostawic sfml do obslugi okna + myszy i klawiatury
    * fajnie byloby dodac takie animacje rowniez do tego projektu symulacji i troche wzbogacic render 
    * fajny bylby loading screen na generowaniu swiata z jakas animacja