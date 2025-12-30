#include <iostream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <mmsystem.h>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#pragma comment(lib, "winmm.lib")

using namespace std;


class ConsoleManager;
class AudioManager;
class PoemScene;
class BattleManager;
class GameManager;
class BattleManager2;

//TITLE SCENE 
class TitleScene {
private:
    const string titleArt = R"(
    ╔══════════════════════════════════════════════╗
    ║     ██╗    ██╗██╗  ██╗██╗████████╗███████╗   ║ 
    ║     ██║    ██║██║  ██║██║╚══██╔══╝██╔════╝   ║
    ║     ██║ █╗ ██║███████║██║   ██║   █████╗     ║                      
    ║     ██║███╗██║██╔══██║██║   ██║   ██╔══╝     ║
    ║     ╚███╔███╔╝██║  ██║██║   ██║   ███████╗   ║
    ║      ╚══╝╚══╝ ╚═╝  ╚═╝╚═╝   ╚═╝   ╚══════╝   ║           
    ║                                              ║                   
    ║  ██████╗   ██╗  ██╗███████╗███████╗███╗   ██╗║
    ║ ██╔═══██╗  ██║  ██║██╔════╝██╔════╝████╗  ██║║
    ║ ██║   ██║  ██║  ██║█████╗  █████╗  ██╔██╗ ██║║
    ║ ██║   ██║  ██║  ██║██╔══╝  ██╔══╝  ██║╚██╗██║║
    ║ ╚████████╗ ║██████║███████╗███████╗██║ ╚████║║
    ║  ╚═══════╝ ╚══════╝╚══════╝╚══════╝╚═╝  ╚═══╝║  
    ║                                              ║
    ║              A s   i t   B e g a n           ║
    ║                                              ║
    ║           ┌──────────────────────┐           ║
    ║           │    1. New Game       │           ║
    ║           │    2. Load Game      │           ║
    ║           │    3. Exit           │           ║
    ║           └──────────────────────┘           ║
    ╚══════════════════════════════════════════════╝
)";

public:
    void display(ConsoleManager& console, AudioManager& audio);
};

//  GAME STATE 
class GameState {
    public:
        enum class Progress {
            TITLE_SCREEN,
            POEM_SCENE,
            IN_BATTLE,
            TIME_JUMP  
        };
    
        static void saveProgress(Progress progress) {
            ofstream file("save.dat", ios::binary);
            if (file) {
                int value = static_cast<int>(progress);
                file.write(reinterpret_cast<char*>(&value), sizeof(value));
            }
        }
    
        static Progress loadProgress() {
            ifstream file("save.dat", ios::binary);
            if (file) {
                int value;
                file.read(reinterpret_cast<char*>(&value), sizeof(value));
                return static_cast<Progress>(value);
            }
            return Progress::TITLE_SCREEN;  
        }
    
        static void deleteSave() {
            remove("save.dat");
        }
    };

//CONSOLE MANAGER 
class ConsoleManager {
public:
    ConsoleManager() {
        SetConsoleOutputCP(CP_UTF8);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
        system("mode con cols=80 lines=30");
    }

    void clearScreen() {
        cout << "\033[2J\033[H";
    }

    void setCursorPosition(int x, int y) {
        cout << "\033[" << y << ";" << x << "H";
    }
};

// AUDIO MANAGER 
class AudioManager {
    public:
        enum class MusicState {
            TITLE,
            BATTLE,
            NOA_THEME,
            LORE,
            BATTLE2,  
            SURPRISE, 
            ASFANEH_THEME,
            NONE
        };

private:
    MusicState currentMusicState;

public:
    AudioManager() : currentMusicState(MusicState::NONE) {}

    void playMusic(MusicState state) {
        stopMusic();
        switch (state) {
            case MusicState::BATTLE:
                PlaySound(TEXT("battle1.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
                currentMusicState = MusicState::BATTLE;
                break;
            case MusicState::NOA_THEME:
                PlaySound(TEXT("noastheme.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
                currentMusicState = MusicState::NOA_THEME;
                break;
            case MusicState::TITLE:
                PlaySound(TEXT("white_queen.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
                currentMusicState = MusicState::TITLE;
                break;
            case MusicState::LORE:
                PlaySound(TEXT("lore.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
                currentMusicState = MusicState::LORE;
                break;
            case MusicState::BATTLE2:  // <-- Add this case
                PlaySound(TEXT("battle2.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
                currentMusicState = MusicState::BATTLE2;
                break;
            case MusicState::SURPRISE: // <-- Add this case
                PlaySound(TEXT("surprise.wav"), NULL, SND_ASYNC | SND_FILENAME);
                currentMusicState = MusicState::SURPRISE;
                break;
            case MusicState::ASFANEH_THEME:
                PlaySound(TEXT("asfanehtheme.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
                currentMusicState = MusicState::ASFANEH_THEME;
                break;
            default:
                currentMusicState = MusicState::NONE;
                break;
        }
    }

    void stopMusic() {
        if (currentMusicState != MusicState::NONE) {
            PlaySound(NULL, NULL, 0);
            currentMusicState = MusicState::NONE;
        }
    }

    bool isPlayingBattleMusic() const {
        return currentMusicState == MusicState::BATTLE;
    }
};

// ==================== TEXT EFFECTS ====================
class TextEffects {
public:
    static void typeText(const string& text, int charDelayMs = 50) {
        for (char c : text) {
            cout << c << flush;
            this_thread::sleep_for(chrono::milliseconds(charDelayMs));
        }
    }

    static void fadeInText(const string& text, int steps = 10, int stepDelay = 100) {
        vector<string> fadeColors = {
            "\033[38;2;25;25;25m", "\033[38;2;51;51;51m",
            "\033[38;2;77;77;77m", "\033[38;2;102;102;102m",
            "\033[38;2;128;128;128m", "\033[38;2;153;153;153m",
            "\033[38;2;179;179;179m", "\033[38;2;204;204;204m",
            "\033[38;2;230;230;230m", "\033[38;2;255;255;255m"
        };

        for (int i = 0; i < steps; i++) {
            cout << "\033[2J\033[H" << fadeColors[i] << text << flush;
            this_thread::sleep_for(chrono::milliseconds(stepDelay));
        }
    }
};

//  POEM SCENE
class PoemScene {
private:
    vector<string> poemLines = {
        "Sing to me of an age long past",
        "Crimson tides and iron crowns",
        "Sword in hand a warrior clutches stone to breast", 
        "In sword etched his fading ambition",
        "In stone his tempered skill",
        "By sword attested, by power revealed",
        "Sing of the..."
    };

public:
void display(ConsoleManager& console, AudioManager& audio) {
    console.clearScreen();
    audio.playMusic(AudioManager::MusicState::TITLE);

    const int totalDurationMs = 42000; // Exactly 42 seconds
    const int lineDelayMs = 1000;
    const int charsPerLine = 30;

    int charDelayMs = max(30, (totalDurationMs - (static_cast<int>(poemLines.size()) * lineDelayMs)) / 
                         (static_cast<int>(poemLines.size()) * charsPerLine));

    for (size_t i = 0; i < poemLines.size(); i++) {
        console.clearScreen();
        cout << "\n\n\n\n\n\n\n               ";
        TextEffects::typeText(poemLines[i], charDelayMs);
        this_thread::sleep_for(chrono::milliseconds(lineDelayMs));
    }
}
};
//Battle Manager 2
class BattleManager2 {
    private:
        ConsoleManager& console;
        AudioManager& audio;
    
        struct Character {
            string name;
            int health;
            int maxHealth;
            int mp;
            int maxMp;
            int attack;
            int defense;
            int hpPellets;
            int blessedNectre;
        };
    
        Character player;
        Character enemy;
    
        string getHealthBar(int current, int max, int segments = 20) {
            string bar;
            int filled = (current * segments) / max;
            for (int i = 0; i < segments; i++) {
                bar += (i < filled) ? "█" : "░";
            }
            return bar;
        }
    
        string generateBattleScreen() {
            return R"(
    ~        ~          ~            ~        ~
    ┌──────────┐                        _.,-'=_.-'-._  ~        ~
    │          │   ~           ~   ._.-'             '-._   ~
    │ 1. Attack│          _.-':_.-'                      '-._   ~     ~
    │ 2. Defend│      _.-'                                   '-._.'-._
    │ 3. Magic │-'.-,'                                                '-.
    │ 4. Item  │'-._                       /\   /\                    _.-'
    │ 5. Flee  │   '-.           (o)(o)  /  \ /  \                ._'
    │          │      '-._         (/      /\ (           _.'-._,-'
    └──────────┘          '-._            /  \ )      _.-'   (o o)
     ~     ) ( ) (    ~     .-'               (     .'       ))~((
          ) " ( " (        .-'                 )    '-._.,.            ~
         )  "  ("  (       '-._               /           '-._  ~ 
        )   "   (   ( ___      '-._          (                '.   ~
            "   "    |   | ~      .'          )                '-._
      ---._-|--|--|--|--/     _.-'           /  (o)(o)           _.'   ~
           \ o  o  o  o/     '-._           /    (  )           '-._-'-.
       ~~~~~~~~~~~~~~~~~         '-._      (                        _.-'
      ~          ~             ~     '-.    ) /\            _.-"._,'
                      ~              _.'   / /\ /\         '.  ~    (o o)
        (o o)              _.-'-._.-'     / /  \  \          '-._._ ))~((
        ))~(( ~        _.-'              /                         '-. ~
                    .-'              .-'('-._                        '-.
     ~            _.'         _.-'-.-'~   ~  '.             _.'-.-'._  .'
         .-'=_.'-'         .-'  ~   ~   _ _.-'          _.-'     ~   '.'
      _.-'                 '-._,.-'._.-'    (o)(o)     '_   ~       ~
    .'                                         \)         '-._   ~    ~
    )";
        }
    
        string generateStatusBox() {
            stringstream ss;
            ss << "┌────────────────────────────────────────────────────────────────────────────────────┐\n"
               << "    │ [Noa]: HP " << getHealthBar(player.health, player.maxHealth) 
               << " " << setw(3) << player.health << "/" << setw(3) << player.maxHealth 
               << "   [Bawarij]: HP " << getHealthBar(enemy.health, enemy.maxHealth)
               << " " << setw(3) << enemy.health << "/" << setw(3) << enemy.maxHealth << "│\n"
               << "    │                                                                                    │\n"
               << "    │        MP " << setw(2) << player.mp << "/" << setw(2) << player.maxMp 
               << "                                                                    │\n"
               << "    └────────────────────────────────────────────────────────────────────────────────────┘";
            return ss.str();
        }
    
        void displayBattleStatus() {
            console.clearScreen();
            cout << generateBattleScreen();
            cout << generateStatusBox();
        }
    
        void displayActionMessage(const string& message) {
            console.setCursorPosition(1, 25);
            cout << "                                                                 ";
            console.setCursorPosition(1, 25);
            cout << message;
            console.setCursorPosition(1, 26);
            cout << "                                                                 ";
            console.setCursorPosition(1, 26);
            cout << "Choose action (1-5): ";
        }
    
        void displayMagicMenu() {
            console.clearScreen();
            cout << generateBattleScreen();
            cout << generateStatusBox();
            cout << R"(
        Magic Spells:
        1. Ignis (20 MP) - 70 damage
        2. Hortensia's Prayer (30 MP) - Restore 200 HP
        Choose spell (1-2) or any other key to cancel: )";
        }
    
        void handlePlayerMagic(char choice) {
            string message;
            switch (choice) {
                case '1': // Ignis
                    if (player.mp >= 20) {
                        player.mp -= 20;
                        enemy.health -= 70;
                        enemy.health = max(0, enemy.health);
                        message = "You cast Ignis for 50 damage!";
                    } else {
                        message = "Not enough MP!";
                    }
                    break;
                case '2': // Hortensia's Prayer
                    if (player.mp >= 30) {
                        player.mp -= 30;
                        player.health = min(player.maxHealth, player.health + 200);
                        message = "You cast Hortensia's Prayer! Restored 50 HP.";
                    } else {
                        message = "Not enough MP!";
                    }
                    break;
                default:
                    return; // Cancel magic selection
            }
            displayBattleStatus();
            displayActionMessage(message);
            this_thread::sleep_for(chrono::milliseconds(1500));
        }
    
        void displayItemMenu() {
            console.clearScreen();
            cout << generateBattleScreen();
            cout << generateStatusBox();
            cout << "\n"
                 << "    Items:\n"
                 << "    1. HP Pellet (" << player.hpPellets << ") - Restore 80 HP\n"
                 << "    2. Blessed Nectre (" << player.blessedNectre << ") - Restore 50 MP\n"
                 << "    Choose item (1-2) or any other key to cancel: ";
        }
        
        void handlePlayerItems(char choice) {
            string message;
            switch (choice) {
                case '1': // HP Pellet
                    if (player.hpPellets > 0) {
                        player.hpPellets--;
                        player.health = min(player.maxHealth, player.health + 80);
                        message = "Used HP Pellet! Restored 80 HP.";
                    } else {
                        message = "No HP Pellets left!";
                    }
                    break;
                case '2': // Blessed Nectre
                    if (player.blessedNectre > 0) {
                        player.blessedNectre--;
                        player.mp = min(player.maxMp, player.mp + 50);
                        message = "Used Blessed Nectre! Restored 50 MP.";
                    } else {
                        message = "No Blessed Nectre left!";
                    }
                    break;
                default:
                    return; // Cancel item selection
            }
            displayBattleStatus();
            displayActionMessage(message);
            this_thread::sleep_for(chrono::milliseconds(1500));
        }
    
        void applyEnemyAttack() {
            int damage = 50;
            if (player.defense) {
                damage /= 2;
                player.defense = 0;
            }
            player.health -= damage;
            player.health = max(0, player.health);
            string message = enemy.name + " attacks you for " + to_string(damage) + " damage!";
            displayBattleStatus();
            displayActionMessage(message);
        }
    
        void enemyTurn() {
            // Decide whether to use a healing item if health is low
            if (enemy.health < enemy.maxHealth * 0.3 && enemy.hpPellets > 0) {
                enemy.hpPellets--;
                enemy.health = min(enemy.maxHealth, enemy.health + 64);
                string message = enemy.name + " uses HP Pellet! Restored 64 HP.";
                displayBattleStatus();
                displayActionMessage(message);
                this_thread::sleep_for(chrono::milliseconds(1500));
                return;
            }
    
            // Regular attack
            applyEnemyAttack();
            this_thread::sleep_for(chrono::milliseconds(1500));
        }
    
        void handleVictory() {
            audio.stopMusic();
            audio.playMusic(AudioManager::MusicState::SURPRISE);
            
            vector<string> victoryDialogue = {
                "[Bawariji]:\"This wasn't worth the money.\"",
                "",
                "[Noa]:\"Looks like someone doesn't want us around...\"",
                "",
                "[Franz]:\"Our presence here was requested by the people of Jazireh Ghdis.\"",
                "        \"It must be forces from the east who attacked us.\"",
                "",
                "[Noa]:\"Jazireh Ghdis is west and Keshor Ahan is east.\"",
                "      \"Is the rift between them really that bad?\"",
                "",
                "[Franz]:\"Its more than just a rift, the hatred between them goes centuries back.\"",
                "",
                "[Noa]:\"So to no one's surprice soldier's from the east crossed into the west.\"",
                "      \"What does your father, governor of East Gandahara, say about this?\"",
                "",
                "[Franz]:\"He doesn't seem to think it means much.\"",
                "        \"it has happened before many times but this time he actually decided to listen to the people's request.\"",
                "        \"My father didn't wanna bother with the main force so he just sent me to deal with it.\"",
                "        \"...\"",
                "        \"Are you worried?\"",
                "",
                "[Noa]:\"Not really, just curious.\"",
                "      \"Will you eventually go back to Takshasila to become a priest of the Church of Gandahara?\"",
                "",
                "[Franz]:\"I don't think that's my choice.\"",
                "        \"My father wants me to inherit his post as the governer of East Gandahara so that he can hold power even after retirement.\"",
                "",
                "Out of nowhere, a sellsword who'd slipped past our knights raised his crossbow, took careful aim at Franz, and loosed the bolt.",
                "",
                "But he wasn't fast enough. Noa shoved Franz clear—yet the bolt still found its mark, sending him tumbling into the churning ocean below."
            };
        
            console.clearScreen();
            
            // Display dialogue with Enter key progression
            for (const auto& line : victoryDialogue) {
                if (line.empty()) {
                    cout << endl; // Empty string means new paragraph
                    continue;
                }
                
                // Type out the line character by character
                TextEffects::typeText(line);
                cout << endl;
                
                // Wait for Enter key
                while (true) {
                    int key = _getch();
                    if (key == '\r') break; // Enter key pressed
                }
            }
            
            // Clear screen and display final scene
    audio.stopMusic();
    audio.playMusic(AudioManager::MusicState::ASFANEH_THEME);
    console.clearScreen();

    // Display the church ASCII art
    cout << R"(
                                   ,|,
                                   |||
                                  / | \
                                  |   |                              
                                 / ___ \
                                /       \ 
                               /     |   |
                8              |     |   |
              ""8""           /      |    \
                8            /        \   ,\
              ,d8888888888888|========|="" |
            ,d"  "88888888888|  ,aa,  |  a |
          ,d"      "888888888|  8  8  |  8 |
       ,d8888888b,   "8888888|  8aa8  |  8,|
     ,d"  "8888888b,   "88888|========|="" |
   ,d"      "8888888b,   "888|  a  a  |  a |
 ,d"   ,aa,   "8888888b,   "8|  8  8  |  8,|
/|    d"  "b    |""""""|     |========|="" |
 |    8    8    |      |     |  ,aa,  |  a |
 |    8aaaa8    |      |     |  8  8  |  8 |
 |              |      |     |  """"  | ,,=|
 |aaaaaaaaaaaaaa|======"""""""""""""""""
)" << endl << endl;

// Display the final scene text
vector<string> finalScene = {
"At midday, a solitary girl stepped through the church's front gate.",
"As was her custom, she made her way to the beach beside the church, intent on collecting seashells.",
"But today, she could not have foreseen what awaited her.",
"He lay unconscious on the sand, dark hair matted across his black armor.",  
"She knelt at his side, breath hitching at the sight of his still form.",  
"Her eyes flicked to the empty shore, then back to his pale face as her heart thundered.",  
"With trembling fingers, she drew her cloak around him, every pulse echoing silent alarm.",
"",
"Now inside the Church.",
"",
"[Noa]:\"Where am I.\"",
"",
"[Afseneh]:\"Try not to talk.\"",
"          \"How do you feel?\"",
"",
"[Noa]:\"Tired, but apart from that I've never been better.\"",
"",
"[Girl]:\"...Good, you've been alseep for 3 days.\"",
"          \"This is a Church in Jazireh Ghdis.",
"          \"You washed up on the beach outside.\"",
"",
"[Noa]:\"Did you save me...Miss?\"",
"",
"[Girl]:\"My name is Asfaneh.\"",
"       \"I'm not a nun but I help out at the Church from time to time.\"",
"",
"[Noa]:\"My name is Noa; A Knight of Gandahara.\"",
"",
"[Asfaneh]:\"Ah...so you are Gandaharan.\"",
"",
"[Noa]:\"We were heading for this island but our ship was caught up in the storm.\"",
"      \"Our ship was then attacked by pirates.\"",
"",
"[Asfaneh]:\"Did you fall overboard?\"",
"",
"[Noa]:\"Something like that. I was never much of a swimmer.\"",
"",
"[Asfaneh]:\"haha..\"",
"          \"There's word of a group of knights with armour like yours in the west.\"",
"          \"Are they perhaps your comrades?\"",
"",
"[Noa]:\"That's gotta Franz and the others.\"",
"      \"I have to get to them.\"",
"",
"[Asfaneh]:\"You can go to them tomorrow, for now rest and regain your strenght.\"",
"          \"Good night...\"",
"",
".............................................................................",
"",
"Thankyou for playing the demo for WHITE QUEEN: As it Began",
"I hope you await the full release in May of 2026."
};


for (const auto& line : finalScene) {
if (line.empty()) {
cout << endl;
continue;
}

TextEffects::typeText(line);
cout << endl;


while (true) {
int key = _getch();
if (key == '\r') break;
}
}

// Return to title screen (music continues playing)
TitleScene title;
title.display(console, audio);

        }
    
    
    public:
        BattleManager2(ConsoleManager& consoleRef, AudioManager& audioRef) 
        : console(consoleRef), audio(audioRef) {
            // Initialize player with items
            player = {"Noa", 300, 300, 50, 50, 45, 0, 5, 2};
            enemy = {"Bawarij", 500, 500, 0, 0, 50, 0, 3};
        }
    
        void playerTurn() {
            displayBattleStatus();
            displayActionMessage("Your turn! Choose your action:");
            
            char choice = _getch();
            
            switch (choice) {
                case '1': // Attack
                    {
                        int damage = player.attack;
                        if (enemy.defense) {
                            damage /= 2;
                            enemy.defense = 0;
                        }
                        enemy.health -= damage;
                        enemy.health = max(0, enemy.health);
                        string message = "You attack for " + to_string(damage) + " damage!";
                        displayBattleStatus();
                        displayActionMessage(message);
                        this_thread::sleep_for(chrono::milliseconds(1500));
                    }
                    break;
                case '2': // Defend
                    player.defense = 1;
                    displayBattleStatus();
                    displayActionMessage("You take a defensive stance!");
                    this_thread::sleep_for(chrono::milliseconds(1500));
                    break;
                case '3': // Magic
                    displayMagicMenu();
                    handlePlayerMagic(_getch());
                    break;
                case '4': // Item
                    displayItemMenu();
                    handlePlayerItems(_getch());
                    break;
                case '5': // Flee
                    if (rand() % 100 < 70) { // 70% chance to succeed
                        displayBattleStatus();
                        displayActionMessage("You successfully fled from battle!");
                        this_thread::sleep_for(chrono::milliseconds(1500));
                        TitleScene title;
                        audio.stopMusic();
                        title.display(console, audio);
                        return;
                    } else {
                        displayBattleStatus();
                        displayActionMessage("Failed to escape!");
                        this_thread::sleep_for(chrono::milliseconds(1500));
                    }
                    break;
                default:
                    displayBattleStatus();
                    displayActionMessage("Invalid choice! Choose 1-5.");
                    this_thread::sleep_for(chrono::milliseconds(1000));
                    playerTurn(); // Recursively call for valid input
                    return;
            }
        }
    
        void startBattle() {
            audio.playMusic(AudioManager::MusicState::BATTLE2);
    
            while (true) {
                // Player turn
                playerTurn();
                if (enemy.health <= 0) {
                    handleVictory();
                    return;
                }
                
                // Enemy turn
                enemyTurn();
                if (player.health <= 0) {
                    // Handle game over (similar to first battle)
                    TitleScene title;
                    audio.stopMusic();
                    title.display(console, audio);
                    return;
                }
            }
        }
    };

// ==================== BATTLE MANAGER ====================
class BattleManager {
private:
    ConsoleManager& console;
    AudioManager& audio;

    struct Character {
        string name;
        int health;
        int maxHealth;
        int mp;
        int maxMp;
        int attack;
        int defense;
        // Add inventory for items
        int hpPellets;
        int divineNectre;
        int daznasBlessing;
    };

    Character player;
    Character enemy;

    string getHealthBar(int current, int max, int segments = 20) {
        string bar;
        int filled = (current * segments) / max;
        for (int i = 0; i < segments; i++) {
            bar += (i < filled) ? "█" : "░";
        }
        return bar;
    }

    string generateBattleScreen() {
        stringstream ss;
        ss << R"(
┌──────────┐                      |>>>                                
│          │                      |                               
│ 1. Attack│        |>>>      _  _|_  _         |>>>              
│ 2. Defend│        |        |;| |;| |;|        |
│ 3. Magic │    _ _ |_  _    \\.    .  /    _  _|_  _
│ 4. Item  │   |;|_|;|_|;|    \\:. ,  /    |;|_|;|_|;|
│ 5. Flee  │   \\..      /    ||;   . |    \\.    .  /
│          │    \\.  ,  /     ||:  .  |     \\:  .  /
└──────────┘     ||:   |_   _ ||_ . _ | _   _||:   |
                 ||:  .|||_|;|_|;|_|;|_|;|_|;||:.  |
                 ||:   ||.    .     .      . ||:  .|
                 ||: . || .     . .   .  ,   ||:   |       \,/
                 ||:   ||:  ,  _______   .   ||: , |            /`\
                 ||:   || .   /+++++++\    . ||:   |
                 ||:   ||.    |+++++++| .    ||: . |
              __ ||: . ||: ,  |+++++++|.  . _||_   |
     ____--`~    '--~~__|.    |+++++__|----~    ~`---,              ___
-~--~                   ~---__|,--~'                  ~~----_____-~'   `~----~~  
)";
        return ss.str();
    }

    string generateStatusBox() {
        stringstream ss;
        ss << "┌──────────────────────────────────────────────────────────────────────────────────────────────────────┐\n"
           << "│ [Noa]: HP " << getHealthBar(player.health, player.maxHealth) 
           << " " << setw(4) << player.health << "/" << setw(4) << player.maxHealth 
           << " [The Dark Knight Qahar]: HP " << getHealthBar(enemy.health, enemy.maxHealth)
           << " " << setw(5) << enemy.health << "/" << setw(5) << enemy.maxHealth << "│\n"
           << "│        MP " << setw(4) << player.mp << "/" << setw(4) << player.maxMp 
           << "                                               MP " << setw(4) << enemy.mp << "/" << setw(4) << enemy.maxMp << "                       │\n"
           << "└──────────────────────────────────────────────────────────────────────────────────────────────────────┘";
        return ss.str();
    }

    void displayBattleStatus() {
        console.clearScreen();
        cout << generateBattleScreen();
        cout << generateStatusBox();
    }

    void displayActionMessage(const string& message) {
        console.setCursorPosition(1, 25);
        cout << "                                                                 ";
        console.setCursorPosition(1, 25);
        cout << message;
        console.setCursorPosition(1, 26);
        cout << "                                                                 ";
        console.setCursorPosition(1, 26);
        cout << "Choose action (1-5): ";
    }

    void displayMagicMenu() {
        console.clearScreen();
        cout << generateBattleScreen();
        cout << generateStatusBox();
        cout << R"(
    Magic Spells:
    1. Ignis (30 MP) - 50 damage
    2. Judgement (70 MP) - 300 damage
    3. Jihad (100 MP) - 700 damage
    Choose spell (1-3) or any other key to cancel: )";
    }

    void handlePlayerMagic(char choice) {
        string message;
        switch (choice) {
            case '1': // Ignis
                if (player.mp >= 30) {
                    player.mp -= 30;
                    enemy.health -= 50;
                    enemy.health = max(0, enemy.health);
                    message = "You cast Ignis for 50 damage!";
                } else {
                    message = "Not enough MP!";
                }
                break;
            case '2': // Judgement
                if (player.mp >= 70) {
                    player.mp -= 70;
                    enemy.health -= 300;
                    enemy.health = max(0, enemy.health);
                    message = "You cast Judgement for 300 damage!";
                } else {
                    message = "Not enough MP!";
                }
                break;
            case '3': // Jihad
                if (player.mp >= 100) {
                    player.mp -= 100;
                    enemy.health -= 700;
                    enemy.health = max(0, enemy.health);
                    message = "You cast Jihad for 700 damage!";
                } else {
                    message = "Not enough MP!";
                }
                break;
            default:
                return; // Cancel magic selection
        }
        displayBattleStatus();
        displayActionMessage(message);
        this_thread::sleep_for(chrono::milliseconds(1500));
    }

    void displayItemMenu() {
        console.clearScreen();
        cout << generateBattleScreen();
        cout << generateStatusBox();
        cout << "\n"
             << "    Items:\n"
             << "    1. HP Pellet (" << player.hpPellets << ") - Restore 64 HP\n"
             << "    2. Divine Nectre (" << player.divineNectre << ") - Restore 200 MP\n"
             << "    3. Dazna's Blessing (" << player.daznasBlessing << ") - Restore 1000 HP\n"
             << "    Choose item (1-3) or any other key to cancel: ";
    }
    
    void handlePlayerItems(char choice) {
        string message;
        switch (choice) {
            case '1': // HP Pellet
                if (player.hpPellets > 0) {
                    player.hpPellets--;
                    player.health = min(player.maxHealth, player.health + 64);
                    message = "Used HP Pellet! Restored 64 HP.";
                } else {
                    message = "No HP Pellets left!";
                }
                break;
            case '2': // Divine Nectre
                if (player.divineNectre > 0) {
                    player.divineNectre--;
                    player.mp = min(player.maxMp, player.mp + 200);
                    message = "Used Divine Nectre! Restored 200 MP.";
                } else {
                    message = "No Divine Nectre left!";
                }
                break;
            case '3': // Dazna's Blessing
                if (player.daznasBlessing > 0) {
                    player.daznasBlessing--;
                    player.health = min(player.maxHealth, player.health + 1000);
                    message = "Used Dazna's Blessing! Restored 1000 HP.";
                } else {
                    message = "No Dazna's Blessing left!";
                }
                break;
            default:
                return; // Cancel item selection
        }
        displayBattleStatus();
        displayActionMessage(message);
        this_thread::sleep_for(chrono::milliseconds(1500));
    }

    void applyEnemyAttack() {
        int damage = enemy.attack;
        if (player.defense) {
            damage /= 2;
            player.defense = 0;
        }
        player.health -= damage;
        player.health = max(0, player.health);
        string message = enemy.name + " attacks you for " + to_string(damage) + " damage!";
        displayBattleStatus();
        displayActionMessage(message);
    }

    void enemyTurn() {
        // Decide whether to use a healing item if health is low
        if (enemy.health < enemy.maxHealth * 0.3) { // Use item if below 30% health
            int itemChoice = rand() % 2;
            if (itemChoice == 0 && enemy.mp < 100) { // Use Agares' Blood if MP is low
                enemy.mp = min(enemy.maxMp, enemy.mp + 400);
                string message = enemy.name + " uses Agares' Blood! Restored 400 MP.";
                displayBattleStatus();
                displayActionMessage(message);
                this_thread::sleep_for(chrono::milliseconds(1500));
                return;
            } else if (itemChoice == 1 && enemy.daznasBlessing > 0) { // Use Dazna's Blessing if available
                enemy.daznasBlessing--;
                enemy.health = min(enemy.maxHealth, enemy.health + 1000);
                string message = enemy.name + " uses Dazna's Blessing! Restored 1000 HP.";
                displayBattleStatus();
                displayActionMessage(message);
                this_thread::sleep_for(chrono::milliseconds(1500));
                return;
            }
        }

        // Randomly select action type
        int actionType = rand() % 10; // 0-4: attack, 5-7: magic, 8-9: special
        
        // Use Ars Goetia if health is very low (desperate measure)
        if (enemy.health < enemy.maxHealth * 0.15) {
            string message = enemy.name + " uses Ars Goetia! " + enemy.name + " sacrifices health to restore 700 MP!";
            enemy.health = max(1, enemy.health - 500); // Ensure enemy doesn't kill themselves
            enemy.mp = min(enemy.maxMp, enemy.mp + 700);
            displayBattleStatus();
            displayActionMessage(message);
            this_thread::sleep_for(chrono::milliseconds(1500));
            return;
        }
        
        // Choose action based on actionType
        if (actionType < 5) { // Physical attacks (50% chance)
            // Regular or powerful attack (existing code)
            if (actionType < 3) {
                // Regular attack
                applyEnemyAttack();
            } else {
                // Powerful attack
                int damage = enemy.attack * 1.5;
                if (player.defense) {
                    damage /= 2;
                    player.defense = 0;
                }
                player.health -= damage;
                player.health = max(0, player.health);
                string message = enemy.name + " uses a powerful attack for " + to_string(damage) + " damage!";
                displayBattleStatus();
                displayActionMessage(message);
                this_thread::sleep_for(chrono::milliseconds(1500));
            }
        } else if (actionType < 8) { // Magic attacks (30% chance)
            // Select one of three magic attacks
            int spellChoice = rand() % 3;
            if (spellChoice == 0 && enemy.mp >= 100) { // Thundara
                enemy.mp -= 100;
                int damage = 200;
                if (player.defense) {
                    damage /= 2;
                    player.defense = 0;
                }
                player.health -= damage;
                player.health = max(0, player.health);
                string message = enemy.name + " casts Thundara for " + to_string(damage) + " damage!";
                displayBattleStatus();
                displayActionMessage(message);
            } else if (spellChoice == 1 && enemy.mp >= 120) { // Gate of Heaven
                enemy.mp -= 120;
                int damage = 400;
                if (player.defense) {
                    damage /= 2;
                    player.defense = 0;
                }
                player.health -= damage;
                player.health = max(0, player.health);
                string message = enemy.name + " casts Gate of Heaven for " + to_string(damage) + " damage!";
                displayBattleStatus();
                displayActionMessage(message);
            } else if (spellChoice == 2 && enemy.mp >= 300) { // Grand Cross
                enemy.mp -= 300;
                int damage = 1000;
                if (player.defense) {
                    damage /= 2;
                    player.defense = 0;
                }
                player.health -= damage;
                player.health = max(0, player.health);
                string message = enemy.name + " casts Grand Cross for " + to_string(damage) + " damage!";
                displayBattleStatus();
                displayActionMessage(message);
            } else {
                // Not enough MP for chosen spell, use regular attack instead
                applyEnemyAttack();
            }
            this_thread::sleep_for(chrono::milliseconds(1500));
        } else { // Special moves (20% chance)
            if (rand() % 2 == 0) { // 50% chance of either special move
                // Dark Flare (existing special attack)
                if (enemy.mp >= 100) {
                    enemy.mp -= 100;
                    int damage = 300;
                    if (player.defense) {
                        damage /= 2;
                        player.defense = 0;
                    }
                    player.health -= damage;
                    player.health = max(0, player.health);
                    string message = enemy.name + " casts Dark Flare for " + to_string(damage) + " damage!";
                    displayBattleStatus();
                    displayActionMessage(message);
                } else {
                    // Not enough MP, use regular attack
                    applyEnemyAttack();
                }
            } else {
                // Defensive stance
                enemy.defense = 1; // Set defense flag
                string message = enemy.name + " takes a defensive stance!";
                displayBattleStatus();
                displayActionMessage(message);
            }
            this_thread::sleep_for(chrono::milliseconds(1500));
        }
    }

    void handleGameOver() {
        console.clearScreen();
        audio.stopMusic();
        
        // Ask to save progress
        console.setCursorPosition(30, 15);
        cout << "Save progress? (y/n): ";
        
        char choice = _getch();
        if (tolower(choice) == 'y') {
            GameState::saveProgress(GameState::Progress::TIME_JUMP);
        } else {
            GameState::deleteSave();
        }
        
        // Transition to "2 Years ago..."
        console.clearScreen();
        audio.playMusic(AudioManager::MusicState::NOA_THEME);
        cout << "\n\n\n\n\n\n\n               ";
        TextEffects::typeText("Two Years ago...");
        this_thread::sleep_for(chrono::seconds(2));
        
        // Clear screen and display ASCII art and script
        console.clearScreen();
        
        // Display ASCII art from opening.txt
        string karachiArt = R"(
                        .             /"\  .           .
                (      /"\           /~"~\/"\         /"\    ____ __    
                 ) /\ /"^"\          /"~"\~"~\  (    /"^"\  //_/_/||           )
                ()/__\/~"~_________[]_~"~"\~"\  _)   /~"~\ /_/_/_/\|          (_
                |/|__|\"~|    THE     | _______|_|______~"/\_/_/_/_\          |Z|
                / |__| \"| RED ~ DEER || ___        ___ |/_/_/_/_/_\\ ________|Z|_      .
               /________\|============|||) (|      |) (||| .-.  .-. ||.--..--..--.|    /"\
               | __  __ ||            |||\_/|      |\_/||| |_|  |_| |||%%||%%||%%|| . /"~"\  *
               ||__||__|||.-. {}{} .-.||===== .;;, =====||==========|||__||__||__||/"\/~"~\ /o\  .
               ||__||__||||#|  /\  |#||| ___ ;;{};; ___ || .-.  .-. ||.--..--..--.|"~"\"~"~/"o"\/"\
               | __  __ |||_|      |_||||) (| <><> |) (||| |#|  |#| |||%%||%%||%%||~"~\~"~"/o"o\"~"\
               ||__||__|||.-. {}{} .-.|||\_/|  /\  |\_/||| |_|  |_| |||__||__||__||"~"~\"~/o"o"o\"~\
               ||__||__||||#|  /\  |#|_!_====      =====||==========_!_o^~o^~o~^o~|~_!_"\"/"o"o"\~"~_!_
               |========|||_|      |_|\O/===============||  ALTAAF  \O/~^o~^o~^o~^|"\O/"~/"o"o"o"\"~\O/
               |   <>   ||===______==._|_._____________ ||   FINE  ._|_.---------.|._|_."/o"o"o"o\~._|_.
               |  GIFT  || (/______)_ ||/     Herbs    \|| JEWELRY  |||APOTHECARY||~"|"~/."o"o"o".\_-| 
               | SHOPPE ||(_/\___/\__ ||UUUUUUUUUUUUUUUU||~^o~oo~o^~||'----------'|- |_-|PORT SIDE|  |
               |.--. .-.|% .----. .--.||.----..--..----.%|.-..--..-.||.--..--..--.|_-|   /"""""""\_ -|
               ||[]| | |%%%|    | |/\||||~~~~||<>||~~~~%%%|_||[]||_||||^^||<>||^^||  |_ -|^^^^^^^|_ -|
               || ;| |_%%%%%____| |- ||||____|| ;||___%%%%%_|| o||_||||__||; ||__|| -|_- /_______\   |
               ||__|__%%%%%%%_____|__|||______|__|___%%%%%%%_|__|___||____|__|____|_[#]_/_________\_[#]
               ~`  `"%%%%%%%%%~"^"`  `~|~"^"~"`  `~"%%%%%%%%%`  `~"^~|~"^~`  `~"^~"^"~"^           ~"~/
               lc_____________________[#]___________________________[#]______________________________/
               ^"~"^"~"^"~"^"~"^"~"^"~"^"~"^"~"^"~"^"~"^"~"^"~"^"~"^"~"^"~"^"~"^"~"^"~"^"~"^"~"^"~"^~ 
                                             THE PORT CITY OF KARACHI        
            )";
        
        cout << karachiArt << endl << endl;
        
        // Display script line by line with typewriter effect, waiting for Enter key
        vector<string> scriptLines = {
            "[Franz]:\"Did your mother complain about this?\"",
            "\n",
            "[Noa]:\"I don't have time for that, Franz.\"",
            "      \"I'm more concerned with my newly elevated status. The name Duval just doesn't sit right with me.\"",
            "      \"I'd rather not have changed it at all.\"",
            "\n",
            "[Franz]:\"Come on, don't be so unfeeling about it. Once the ship departs you won't be able to see her for a while.\"",
            "\n",
            "[Noa]:\"We're finally leaving, I've never been abroad before.\"",
            "\n",
            "[Franz]:\"This isn't a vacation, Noa!\"",
            "        \"I don't think you quite grasp our situation here.\"",
            "\n",
            "[Noa]:\"Well, excuse me, commander de Morcef.\"",
            "\n",
            "[Franz]:\"I didn't bring you here for your sarcasm...\"",
            "        \"This might be my first time in command, but I have high hopes for you.\"",
            "\n",
            "[Noa]:\"Yes, SIR!\"",
            "\n",
            "A woman clad in black beckoned to Noa",
            "\n",
            "[Mysterious Fortune Teller]:\"Would you like your fortune told?\"",
            "\n",
            "[Franz]:\"We don't have time for such games.\"",
            "\n",
            "[Noa]:\"...\"",
            "\n",
            "[Franz]:\"You're not acting like yourself. Why the sudden interest in fortune telling?\"",
            "         \"We don't have much time so make it quick.\"",
            "\n",
            "[Fortune teller]:\"Everyone seeks a peek into whats to come.\"", 
            "                 \"The path you seek might not be the one you desire.\"",
            "                 \"Destinies materialize within your thoughts,murmuring in the hidden depths of your mind.\"",
            "                 \"They take shape and cry out, 'Let it begin.'\""
        };
        
        for (const auto& line : scriptLines) {
            // Type out the line character by character
            for (char c : line) {
                cout << c << flush;
                this_thread::sleep_for(chrono::milliseconds(30));
            }
            cout << endl;
            
            // Wait for Enter key
            while (true) {
                int key = _getch();
                if (key == '\r') break; // Enter key pressed
            }
        }
    
        // Clear screen after "Let it begin" and display tarot choices
        console.clearScreen();
    
        // Include all tarot choices directly in the code in order
        vector<string> tarotCards = {
            R"(O: The Fool – "As a bird takes wing."
    You are about to embark on a long journey. Whom do you want at your side?
    1)My family 2)My beloved 3)My friend)",
    
            R"(I: The Magician – "For all things, a beginning."
    You brew an arcane elixir. What power will it hold?
    1)Order and Obedience 2)Concession and Regret 3)Liberty and Turmoil)",
    
            R"(II: The High Priestess – "The chains of faith bind the heart."
    All men have flaws. Which of yours would you remedy?
    1)My Sloth 2)My Envy 3)My Pride)",
    
            R"(III: The Empress – "And she shone like the sun."
    In what thing do you stand above all others?
    1)My Beauty 2)My Wealth 3)My Wisdom)",
    
            R"(IV: The Emperor – "Heavy rests the crown."
    The people of your kingdom rise against you. What course do you take?
    1)Treat for Peace 2)Rally my Armies 3)Abdicate the Throne)",
    
            R"(V: The Hierophant – "Order is a golden shackle."
    Of the many evils men do, which is the most necessary?
    1)Deceiving others 2)Sacrificing others 3)Stealing from others)",
    
            R"(VI: The Lovers – "Look to the man in the mirror."
    What do you seek in a lover?
    1)Wisdom and Ability 2)Wealth and Power 3)Youth and Beauty)",
    
            R"(VII: The Chariot – "Means and ends."
    How do you define victory?
    1)Means & Ends 2)Ending the conflict 3)Preserving my allies)",
    
            R"(VIII: Strength – "No heart is free of shadow."
    All men hold darkness in their hearts. What lurks in yours?
    1)Vengeance for Country 2)Vengeance for Friends 3)Vengeance for Family)",
    
            R"(IX: The Hermit – "Have no fear of failure."
    A sage will grant you a single piece of wisdom. What would you learn of him?
    1)The secret of Wealth 2)The secret of Victory 3)The secret of Winning Hearts)",
    
            R"(X: Wheel of Fortune – "Opportunity knocks but once."
    You stand at the end of a hard-fought battle. Why have your forces lost?
    1)Inadequate troops 2)Ill Luck 3)Poor Leadership)",
    
            R"(XI: Justice – "Truth is an exacting ally."
    How would you bring together two disparate clans?
    1)By Religion 2)By Diplomacy 3)By Force)",
    
            R"(XII: The Hanged Man – "It is our nature to take."
    You stand poised to take the life of another. Why do you attempt this dire act?
    1)To defend my Life 2)To defend my Honor 3)To satisfy Revenge)",
    
            R"(XIII: Death – "Our path is beset by doubt."
    In the final reckoning, who will prove false?
    1)My King 2)My dearest friend 3)My Beloved)",
    
            R"(XIV: Temperance – "The price of betrayal."
    Your reckless plan has ended in disaster. What has it cost you?
    1)My friend and beloved 2)My position and honor 3)My family and fortune)",
    
            R"(XV: The Devil – "All must bear indignity."
    A devil will grant you any wish... for a price. What are you willing to endure?
    1)Grotesque disfigurement 2)Complete isolation 3)Relentless fear)",
    
            R"(XVI: The Tower – "God is cruel to prince and pauper alike."
    A fire engulfs your home. Whom do you save from the flames?
    1)My child 2)My beloved 3)My parents)",
    
            R"(XVII: The Star – "In the void, a spark of light."
    Stars shoot across the night sky. For what do you wish?
    1)Success for my friends 2)Happiness for myself 3)Peace for the world)",
    
            R"(XVIII: The Moon – "Immorality, thy name is desire."
    In a fit of passion, you take your friend's lover. What will you do next?
    1)Keep the lover for my own 2)Savor the moment 3)Put it out my mind)",
    
            R"(XIX: The Sun – "A lasting peace."
    You stand at the end of a hard-fought battle. To what do you owe your victory?
    1)The strength of my allies 2)My own ability 3)The grace of God)",
    
            R"(XX: Judgement – "The decision shapes the man."
    The man who slew your family stands chained before you. What punishment will you mete out?
    1)Imprisonment 2)Beheading 3)Banishment)",
    
            R"(XXI: The World – "Past is prelude to now."
    What is god to you?
    1)An exemplar 2)A weapon 3)An illusion)"
        };
    
        // Go through all tarot cards in sequence
        for (const auto& card : tarotCards) {
            console.clearScreen();
            
            // Display the card with typewriter effect
            console.setCursorPosition(10, 5);
            TextEffects::typeText(card);
    
            // Get user choice (just 1, 2, or 3)
            console.setCursorPosition(10, 10);
            cout << "Press 1, 2, or 3: ";
            
            char tarotChoice;
            while (true) {
                tarotChoice = _getch();
                if (tarotChoice >= '1' && tarotChoice <= '3') break;
            }
    
            // Display selection result briefly
            console.setCursorPosition(10, 12);
            TextEffects::typeText("You chose: " + string(1, tarotChoice));
            this_thread::sleep_for(chrono::milliseconds(20));
        }
    
        // After tarot cards
    audio.stopMusic();
    console.clearScreen();

    // Play lore music and display initial text
    audio.playMusic(AudioManager::MusicState::LORE);
    cout << "\n\n\n\n\n\n\n               ";
    TextEffects::typeText("As the first light of dawn arrives, roots sink deep into the earth, and water wells forth from its source.");
    this_thread::sleep_for(chrono::seconds(3));

    // Clear screen and display lore using exact requested order
    console.clearScreen();
    
    vector<string> loreScript = {
        "Five years ago, during a time of turmoil, the Gahdaharan Republic was overthrown in a coup d'état.", 
        "Led by General Qahar Khan—commander of the Gandaharan Army—the conspirators stormed the Senate building, slaughtering everyone inside.",
        "",
        "With absolute control secured, Qahar appointed the High Priestess Hortensia as the new ruler of Gandahra.", 
        "Thus transforming the republic into a rigid military led theocracy..", 
        "Founding what is now known as the Holy Gandaharan Empire under the banner of the White Queen.",
        "",
        "Soon after that, the Empire began its conquest into neighbouring lands.",
        "Troops from Takshasila marched into regions that resisted, in hopes of spreading their new found ideals in the name of their queen.",
        "But it was indeed an invasion, not a reformation.",
        "",
        "Countless conflicts erupted: blood spilled, villages burned, and lands fell under occupation.", 
        "But, even in the sea of resistance there were those who submitted to the Gandaharans.",
        "They would come under the empire but still maintain a level of autonomy.",
        "The empire also pledged to support all their needs.",
        "",
        "Some rulers chose to surrender to put an end to meaningless sacrifice.", 
        "It was the smart decision as the might of the empire was too much to bear for most.",
        "Despite all this, compared to the rest of the era, it was a time of relative peace.",
        "\n"
    };

    // Display with identical behavior to scriptLines
    int startRow = 5; // Starting row for text
    for (const auto& line : loreScript) {
        console.setCursorPosition(10, startRow++);
        
        if (line.empty()) {
            // Extra space between paragraphs
            startRow++;
            continue;
        }
        
        // Type out with same effect as scriptLines
        TextEffects::typeText(line);
        
        // Wait for Enter key (same as scriptLines)
        console.setCursorPosition(10, startRow);
        cout << " ";
        while (_getch() != '\r') {}
        console.setCursorPosition(10, startRow);
        cout << "            "; // Clear "Press Enter" text
    }

    // After lore text is finished
    audio.stopMusic();
    console.clearScreen();

    // Play ship music and display ship art
    audio.playMusic(AudioManager::MusicState::NOA_THEME); // Or create a SHIP music state if needed
    PlaySound(TEXT("ship.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);

    // Display ship ASCII art (from shipscene.txt)
    vector<string> shipArt = {
        "#############################################################################",
        "#(@@@@)                    (#########)                   (@@@@@@@@(@@@@@@@@@#",
        "#@@@@@@)___                 (####)~~~   /\\                ~~(@@@@@@@(@@@@@@@#",
        "#@@@@@@@@@@)                 ~~~~      /::~-__               ~~~(@@@@@@@@)~~#",
        "#@@@)~~~~~~                           /::::::/\\                  ~~(@@@@)   #",
        "#~~~                              O  /::::::/::~--,                 ~~~~    #",
        "#                                 | /:::::/::::::/{                         #",
        "#                 |\\              |/::::/::::::/:::|                        #",
        "#                |:/~\\           ||:::/:::::/::::::|                        #",
        "#               |,/:::\\          ||/'::: /::::::::::|                       #",
        "#              |#__--~~\\        |'#::,,/::::::::: __|   ,,'`,               #",
        "#             |__# :::::\\       |-#\"\":::::::__--~~::| ,'     ',     ,,      #",
        "#,    ,,     |____#~~~--\\,'',.  |_#____---~~:::::::::|         ',','  ',    #",
        "# '.,'  '.,,'|::::##~~~--\\    `,||#|::::::_____----~~~|         ,,,     '.''#",
        "#____________'----###__:::\\_____||#|--~~~~::::: ____--~______,,''___________#",
        "#^^^  ^^^^^   |#######\\~~~^^O, | ### __-----~~~~_____########'  ^^^^  ^^^   #",
        "#,^^^^^','^^^^,|#########\\_||\\__O###~_######___###########;' ^^^^  ^^^   ^^ #",
        "#^^/\\/^^^^/\\/\\^^|#######################################;'/\\/\\/^^^/\\/^^^/\\/^#",
        "#   /\\/\\/\\/\\/\\  /\\|####################################'      /\\/\\/\\/\\/\\    #",
        "#\\/\\/\\     /\\/\\/\\  /\\/\\/\\/\\    /\\/\\/\\/\\/\\   /\\/\\/\\    /\\/\\/\\/\\      /\\/\\/\\/\\#",
        "#spb\\/\\/\\    /\\/\\/\\/\\    /\\/\\/\\/\\    /\\/\\/\\/\\   /\\/\\/\\/\\    /\\/\\/\\/\\/\\      #",
        "#############################################################################"
    };

    // Display ship art
    console.clearScreen();
    for (const auto& line : shipArt) {
        cout << line << endl;
    }
    cout << endl;

    // Display ship dialogue line by line
    vector<string> shipDialogue = {
        "Their ship faced some damages during a storm causing them to dock at a random location.",
        "With rain pouring Noa could could do nothing but complain.",
        "",
        "[Noa]:\"Franz we'll never get to the island at this rate.\"",
        "      \"Yes, the ship can be repaired but it's gonna take a while.\"",
        "",
        "[Franz]:\"Is the captain fairing any better than her ship?\"",
        "",
        "[Noa]:\"She's doing fine.\"",
        "      \"But she says to wait out the storm before attempting any repairs.\"",
        "",
        "[Franz]:\"Well there's nothing else we can do about this situation.\"",
        "        \"I didn't think the Monsoon would strike this soon.\"",
        "        \"We've been at sea for so long that I've lost all sense of direction.\"",
        "        \"We should thank Hortensia that the ship didn't sink.\"",
        "",
        "[Gandaharan Soldier]:\"Commander, we're under attack.\"",
        "",
        "[Noa]:\"At a time like this??\"",
        "",
        "[Pirate Bawarij]:\"That crest and armour...\"",
        "                 \"You must be from Gandahara!\"",
        "                 \"Someone paid good money to make sure you don't land on this island.\"",
        "                 \"Archers get them!!\"",
        "",
        "[Franz]:\"Noa, you get their leader.\"",
        "        \"The others and I will take out the archers.\"",
        "",
        "[Noa]:\"Right Sir.\""
    };

    // Display dialogue with Enter key progression
    for (const auto& line : shipDialogue) {
        if (line.empty()) {
            cout << endl;
            continue;
        }
        
        // Type out the line character by character
        
        for (char c : line) {
            cout << c << flush;
            this_thread::sleep_for(chrono::milliseconds(30));
        }
        cout << endl;
        
        // Wait for Enter key
        while (true) {
            int key = _getch();
            if (key == '\r') break; // Enter key pressed
        }
    }

    // Clear screen and start second battle
    audio.stopMusic();
    console.clearScreen();
    BattleManager2 battle2(console, audio);
    battle2.startBattle();
}

public:
    BattleManager(ConsoleManager& consoleRef, AudioManager& audioRef) 
    : console(consoleRef), audio(audioRef) {
        // Initialize player with items
        player = {"Noa", 3200, 3200, 200, 200, 150, 0, 10, 1, 1};
        enemy = {"The Dark Knight Qahar", 10000, 10000, 500, 500, 140, 0, 0, 0, 1};
        srand(static_cast<unsigned int>(time(nullptr)));
    }

    void playerTurn() {
        displayBattleStatus();
        displayActionMessage("Your turn! Choose your action:");
        
        char choice = _getch();
        
        switch (choice) {
            case '1': // Attack
                {
                    int damage = player.attack;
                    if (enemy.defense) {
                        damage /= 2;
                        enemy.defense = 0;
                    }
                    enemy.health -= damage;
                    enemy.health = max(0, enemy.health);
                    string message = "You attack for " + to_string(damage) + " damage!";
                    displayBattleStatus();
                    displayActionMessage(message);
                    this_thread::sleep_for(chrono::milliseconds(1500));
                }
                break;
            case '2': // Defend
                player.defense = 1;
                displayBattleStatus();
                displayActionMessage("You take a defensive stance!");
                this_thread::sleep_for(chrono::milliseconds(1500));
                break;
            case '3': // Magic
                displayMagicMenu();
                handlePlayerMagic(_getch());
                break;
            case '4': // Item
                displayItemMenu();
                handlePlayerItems(_getch());
                break;
            case '5': // Flee
                if (rand() % 100 < 70) { // 70% chance to succeed
                    displayBattleStatus();
                    displayActionMessage("You successfully fled from battle!");
                    this_thread::sleep_for(chrono::milliseconds(1500));
                    TitleScene title;
                    audio.stopMusic();
                    title.display(console, audio);
                    return;
                } else {
                    displayBattleStatus();
                    displayActionMessage("Failed to escape!");
                    this_thread::sleep_for(chrono::milliseconds(1500));
                }
                break;
            default:
                displayBattleStatus();
                displayActionMessage("Invalid choice! Choose 1-5.");
                this_thread::sleep_for(chrono::milliseconds(1000));
                playerTurn(); // Recursively call for valid input
                return;
        }
    }

    void startBattle() {
        audio.playMusic(AudioManager::MusicState::BATTLE);
        GameState::saveProgress(GameState::Progress::IN_BATTLE);

        while (true) {
            // Player turn
            playerTurn();
            if (enemy.health <= 0) {
                displayBattleStatus();
                displayActionMessage("You defeated " + enemy.name + "!");
                this_thread::sleep_for(chrono::seconds(2));
                break;
            }
            
            // Enemy turn
            enemyTurn();
            if (player.health <= 0) {
                handleGameOver();
                return;
            }
        }

        audio.stopMusic();
        audio.playMusic(AudioManager::MusicState::TITLE);
    }
};



//  GAME MANAGER
class GameManager {
public:
    void newGame(ConsoleManager& console, AudioManager& audio) {
        GameState::deleteSave();
        console.clearScreen();
        TextEffects::typeText("\n\n\n\n\n\n\n               Beginning new adventure...");
        this_thread::sleep_for(chrono::seconds(2));
        
        // Skip poem scene for new games
        BattleManager battle(console, audio);
        battle.startBattle();
    }

    void loadGame(ConsoleManager& console, AudioManager& audio) {
        GameState::Progress progress = GameState::loadProgress();
        
        switch (progress) {
            case GameState::Progress::TIME_JUMP:
                console.clearScreen();
                audio.playMusic(AudioManager::MusicState::NOA_THEME);
                cout << "\n\n\n\n\n\n\n               ";
                TextEffects::typeText("2 Years ago...");  // Consistent style
                this_thread::sleep_for(chrono::seconds(2));
                break;
                
            default:
                console.clearScreen();
                TextEffects::typeText("\n\n\n\n\n\n\n               No saved game found!");
                this_thread::sleep_for(chrono::seconds(2));
                break;
        }
        
        TitleScene title;
        title.display(console, audio);
    }
};

//TITLE SCENE 
void TitleScene::display(ConsoleManager& console, AudioManager& audio) {
    console.clearScreen();
    // Music continues from poem scene
    TextEffects::fadeInText(titleArt);
    
    while (true) {
        console.setCursorPosition(0, 0);
        cout << titleArt;
        console.setCursorPosition(36, 27);
        cout << "Select option (1-3): ";
        
        char choice = _getch();
        GameManager game;
        
        switch (choice) {
            case '1': 
                game.newGame(console, audio);
                return;
            case '2':
                game.loadGame(console, audio);
                return;
            case '3':
                audio.stopMusic();
                exit(0);
            default:
                console.setCursorPosition(36, 28);
                cout << "Invalid choice! Press 1-3";
                this_thread::sleep_for(chrono::milliseconds(1000));
        }
    }
}

//MAIN GAME 
class WhiteQueenGame {
private:
    ConsoleManager console;
    AudioManager audio;
    TitleScene titleScene;
    PoemScene poemScene;

public:
    void run() {
        // Initial poem sequence 
        poemScene.display(console, audio);
        
        // Transition to title screen 
        titleScene.display(console, audio);
    }
};


int main() {
    WhiteQueenGame game;
    game.run();
    return 0;
}



