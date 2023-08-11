# Wymagania Impulsator 2.0
1. Rozszerzyć funkcjonalności impulsatora 1.0
    1. problem z projektem (nie buduje się), najprawdopodbniej brakuje plikow naglowkowych
    2. spytac sie Remka co zrobic  
    3. Napisac swoją wersje impulssatora 1.0 (prostą), nastepnie wprowadzać punkty ponizej (PVD i flash)
    4. Schemat wersji finalnej w Kicadzie
    
2. Impulsator musi generować impulsy jako stany High-Z i GND
    1. impuls na PULSE nabija się w momencie wystąpienia obu wejściach stanu GND
    2. Hi-Z to brak impulsu
3. Impulsator musi mieć funkcję zabezpieczającą przed utratą danych np. przez brak prądu
    1. Zastosować zasilanie główne z komputera poprzez USB
    2. Zastosować zasilanie dodatkowe z baterii 3,6V
    3. Zasilanie główne momentalnie zmienia się na bateryjne w momencie utraty zasilania głównego - tak aby nie utracić danych
    4. Po zmianie zasilania na bateryjne zatrzymywany jest proces generowania impulsów
    5. Po zatrzymaniu procesu generowania impulsów ostatnia wartość zapisywana jest do pamięci nieulotnej (FLASH)
    6. Możliwy jest odczyt powyższej wartości poprzez komendę po UART
    7. SLEEP zamienic na STANDBY (program jest budzony przez przerwania)
4. Impulsator zatrzymywany i uruchamiany poprzez komendy po UART
    1. aby impulsator zaczął generować impulsy musi zostać wydana komenda inicjalizująca, tak samo zatrzymanie
    2. odczytanie wartości zapisanej we flash powinno być możliwe poprzez inną komendę
    3. skonfigurowanie ustawień powinno być także możliwe poprzez komendy
    4. rozpoczęcie pracy od zapisanej wartosci w flash, na przykład ustawiamy 30, 25 impulsow nabiło przeszlo na bateryjne, wtedy przy powrocie do zasilania prez kabel dokonczy do 30  (opcjonalne)
    5. dodanie uart4 jako komunikacja przy zasilaniu bateryjnym
    6. uart2 jako komunikacja przy zasilaniu przez kabel
5. Konfiguracja ustawień generowania impulsów - możliwe powinno być ustawienie(moze być w pliku txt):
    1. liczby impulsów lub ich nieskończoność
    2. częstotliwości impulsów
    3. wypełnienia (duty cycle)
6. Świecenie diodami:
    1. kiedy wykryto brak zasilania głównego na odwrót
    2. kiedy zatrzymano generowanie impulsów
    3. w przypadku innych błędów - jakich?
    4. Niski stan naładowania baterii
    5. wylaczyc ledy
7. Opcjonalnie:
    1. UI komunikujący o liczbie nabitych impulsów po zatrzymaniu nabijania impulsów, np. w postaci wyświetlacza albo LEDów wyświetlających stan licznika w postaci liczby binarnej
        1. w takiej konfiguracji bateria powinna wytrzymać 3 doby
8. Mechaniczne:
    1. obmyślone ułożenie PULSE'ów wokół impulsatora w taki sposób, aby nie zwierały się ze sobą i była możliwość wymiany PULS'ów
    2. kabelki łączące impulsator z PULSEm powinny być połączone w pewny sposób, tak, aby niemożliwe było ich przypadkowe wyciągnięcie i żeby nie występowały drgania na zestyku -terminal blocks (mozliwe. ze bedzie trzeba wywiercic dziurki)
    3. przemyślana opcja rozszerzenia ilości wyjść impulsatora, tak aby w przyszłości możliwe było dołożenie większej ilości PULS'ów
    4. Zniwelowac utrade danych przy przejsu na zasilanei bateryjne
    5. Kondesator, dioda6
    6. inforamcja o stanie baterii

Czwartek prezentacja o 11:00
bateria zisila natychmiastowo po zmianie źródła zasilania 
trzeba wykorzystac adc do zbadania czy przy usb stm pobiera z baterii

