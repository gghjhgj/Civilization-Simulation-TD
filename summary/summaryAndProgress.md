~~ToDo symulacja

1) Render://caly zrobiony
    1) ruch kamery i zmiana view przy nim i tego co renderujemy// jest
    2) render i dirty render tylko sf::view// jest 

2) World://caly zrobiony
    - zwiekszyc rozmiar swiata do 3-4mld pol i przeniesc go na heapa//jest
2.5) bledy:
    - spadek ticksow na s przy duzej liczbie zasobow// jest
    - poprawic spawn kamienia// jest
    - cos z liczbami budynkow znowu sie psuje// jest
    - wielka scina na poczatku ale no to nie krytyczne moze zostac i to samo przy pierwszym wsadzie (chyba)
3) Ludzie:
    - po zmianie wielkosci swiata trzeba bedzie poprawic ich decission making zeby nie lazily po calej mapie losowo bo mi cache missy wywali i zdechnie program
    - przesledzic wtedy ruchy - czy dalej robia dziwne rzeczy jesli chodzi o tworzenie paskow etc

4) Cywilizacja:
    - poprawic decission making zeby cywilizacja byla madrzejsza
    - dodac mechanike glodu - czyli poprostu cel = zwiekszenie liczby ludzi najbardziej jak sie da przy jednoczesnym nie wymarciu z glodu 

7) stricte clean code:
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