#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <random>
#include <chrono>
#include <list>
#include <memory>
#include <android/window.h>
//#if defined(USE_JNI)
// These headers are only needed for direct NDK/JDK interaction
#include <jni.h>
#include <android/native_activity.h>

// Since we want to get the native activity from SFML, we'll have to use an
// extra header here:
#include <SFML/System/NativeActivity.hpp>

// NDK/JNI sub example - call Java code from native code

int vibrate(sf::Time duration)
{
    // First we'll need the native activity handle
    ANativeActivity *activity = sf::getNativeActivity();
 //   activity->callbacks->onPause(sf::getNativeActivity());
	
    // Retrieve the JVM and JNI environment
    JavaVM* vm = activity->vm;
    JNIEnv* env = activity->env;

    // First, attach this thread to the main thread
    JavaVMAttachArgs attachargs;
    attachargs.version = JNI_VERSION_1_6;
    attachargs.name = "NativeThread";
    attachargs.group = NULL;
     jint res = vm->AttachCurrentThread(&env, &attachargs);

    if (res == JNI_ERR)
        return EXIT_FAILURE;

    // Retrieve class information
    jclass natact = env->FindClass("android/app/NativeActivity");
    jclass context = env->FindClass("android/content/Context");
    
    // Get the value of a constant
    jfieldID fid = env->GetStaticFieldID(context, "VIBRATOR_SERVICE", "Ljava/lang/String;");
    jobject svcstr = env->GetStaticObjectField(context, fid);
    
    // Get the method 'getSystemService' and call it
    jmethodID getss = env->GetMethodID(natact, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    jobject vib_obj = env->CallObjectMethod(activity->clazz, getss, svcstr);
    
    // Get the object's class and retrieve the member name
    jclass vib_cls = env->GetObjectClass(vib_obj);
    jmethodID vibrate = env->GetMethodID(vib_cls, "vibrate", "(J)V"); 
    
    // Determine the timeframe
    jlong length = duration.asMilliseconds();
    
    // Bzzz!
    env->CallVoidMethod(vib_obj, vibrate, length);
    
    // Free references
    env->DeleteLocalRef(vib_obj);
    env->DeleteLocalRef(vib_cls);
    env->DeleteLocalRef(svcstr);
    env->DeleteLocalRef(context);
    env->DeleteLocalRef(natact);
    
    // Detach thread again
    vm->DetachCurrentThread();
	
	return 0;
}

//#endif

void AutoHideNavBar()
{     
    ANativeActivity* activity = sf::getNativeActivity();
    JavaVM* vm = activity->vm;
    JNIEnv* env = activity->env;

    JavaVMAttachArgs attachargs;
    attachargs.version = JNI_VERSION_1_6;
    attachargs.name = "NativeThread";
    attachargs.group = NULL;
    jint res = vm->AttachCurrentThread(&env, &attachargs);

    if (res == JNI_ERR)
         return;

    activity->vm->AttachCurrentThread(&env, NULL);

    jclass activityClass = env->FindClass("android/app/NativeActivity");
    jmethodID getWindow = env->GetMethodID(activityClass, "getWindow", "()Landroid/view/Window;");

    jclass windowClass = env->FindClass("android/view/Window");
    jmethodID getDecorView = env->GetMethodID(windowClass, "getDecorView", "()Landroid/view/View;");

    jclass viewClass = env->FindClass("android/view/View");
    jmethodID setSystemUiVisibility = env->GetMethodID(viewClass, "setSystemUiVisibility", "(I)V");

    jobject window = env->CallObjectMethod(activity->clazz, getWindow);

    jobject decorView = env->CallObjectMethod(window, getDecorView);

    jfieldID flagFullscreenID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_FULLSCREEN", "I");
    jfieldID flagHideNavigationID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_HIDE_NAVIGATION", "I");
    jfieldID flagImmersiveStickyID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_IMMERSIVE_STICKY", "I");

    const int flagFullscreen = env->GetStaticIntField(viewClass, flagFullscreenID);
    const int flagHideNavigation = env->GetStaticIntField(viewClass, flagHideNavigationID);
    const int flagImmersiveSticky = env->GetStaticIntField(viewClass, flagImmersiveStickyID);
    const int flag = flagFullscreen | flagHideNavigation | flagImmersiveSticky;

    env->CallVoidMethod(decorView, setSystemUiVisibility, flag);   
    activity->vm->DetachCurrentThread();
}




double random_numbers(double,double);


class Head
{
    sf::Texture t_head;
public:
    int i = 30;
    int j = 15;
    sf::Sprite s_head;
    int x;
    int y;
    Head();
};

Head::Head():x(360),y(680)
{
    t_head.loadFromFile("head.png");
    s_head.setTexture(t_head);
    s_head.setPosition(x,y);
}

class Fruit
{
    sf::Texture t_apple;   
public:
    int total;
    sf::Sprite s_apple;
    Fruit();
};


Fruit::Fruit():total(0)
{
    t_apple.loadFromFile("apple.png");
    s_apple.setTexture(t_apple);
}


class Snake
{
    sf::Texture t_body;
public:
    sf::Sprite s_body;
    int x;
    int y;
    Snake(int,int);
    Snake() { };
};

Snake::Snake(int a, int b) :x(a), y(b)
{
    t_body.loadFromFile("body.png");
    s_body.setTexture(t_body);
    s_body.setPosition(x,y);
}

class Field:public sf::Drawable
{
   
public:
    sf::Sprite s_filed;
    sf::Sprite s_up_button;
    sf::Sprite s_down_button;
    sf::Sprite s_right_button;
    sf::Sprite s_left_button;
    sf::Texture t_field;
    sf::Texture t_up_button;
    sf::Texture t_down_button;
    sf::Texture t_right_button;
    sf::Texture t_left_button;
    Field();
    virtual void draw(sf::RenderTarget&,sf::RenderStates)const;
};

void Field::draw(sf::RenderTarget& a,sf::RenderStates)const
{
    a.draw(s_filed);
    a.draw(s_up_button);
    a.draw(s_down_button);
    a.draw(s_right_button);
    a.draw(s_left_button);
}

Field::Field()
{
    t_field.loadFromFile("field.jpg");
    s_filed.setTexture(t_field);

    t_up_button.loadFromFile("up.png");
    s_up_button.setTexture(t_up_button);
    s_up_button.setPosition(295,1200);

    t_down_button.loadFromFile("down.png");
    s_down_button.setTexture(t_down_button);
    s_down_button.setPosition(295,1385);

    t_right_button.loadFromFile("right.png");
    s_right_button.setTexture(t_right_button);
    s_right_button.setPosition(390, 1295);

    t_left_button.loadFromFile("left.png");
    s_left_button.setTexture(t_left_button);
    s_left_button.setPosition(195,1295);
}

class Purple_flower
{
    sf::Texture t_purple_flower;
    int x;
    int y;
public:
    Purple_flower(int,int);
    sf::Sprite s_purple_flower;
};

Purple_flower::Purple_flower(int a,int b):x(a),y(b)
{
    t_purple_flower.loadFromFile("purpleflower.png");
    s_purple_flower.setTexture(t_purple_flower);
    s_purple_flower.setPosition(x,y);
}

class Buttons:public sf::Drawable
{
   
public:
    sf::Texture t_button_play;
    sf::Texture t_button_one;
    sf::Texture t_button_two;
    sf::Texture t_button_three;
    sf::Sprite s_button_one;
    sf::Sprite s_button_two;
    sf::Sprite s_button_three;   
    sf::Sprite s_button_play;
    bool show;
    bool check;
    bool show_button_play;
    virtual void draw(sf::RenderTarget&,sf::RenderStates)const;
    Buttons();
};

void Buttons::draw(sf::RenderTarget& target, sf::RenderStates)const
{   
    
    if (show)
    {
        target.draw(s_button_three);
        target.draw(s_button_two);
        target.draw(s_button_one);
    }
    if (show_button_play)
    {
        target.draw(s_button_play);
    }
    
}

Buttons::Buttons():show(false),check(true),show_button_play(false)
{
    t_button_one.loadFromFile("button1_eng.png");
    t_button_two.loadFromFile("button2_eng.png");
    t_button_three.loadFromFile("button3_eng.png");
    s_button_one.setTexture(t_button_one);
    s_button_two.setTexture(t_button_two);
    s_button_three.setTexture(t_button_three);
    s_button_one.setPosition(130,380);
    s_button_two.setPosition(130,500);
    s_button_three.setPosition(130,620);
    t_button_play.loadFromFile("play_button.png");
    s_button_play.setTexture(t_button_play);
    s_button_play.setPosition(555,1285);
}

class Text_score
{
    sf::Font font;
    int x;
    int y;
public:
    int size;
    std::string str;
    bool draw_text = false;
    sf::Text text_score;
    Text_score(int,int);
};

Text_score::Text_score(int a,int b):x(a),y(b)
{
    font.loadFromFile("19144.ttf");
    text_score.setFont(font);
    text_score.setString(L"0");
    text_score.setCharacterSize(40);
    sf::Color color(0, 255, 255);
    text_score.setFillColor(color);
    text_score.setOutlineThickness(2);
    text_score.setStyle(sf::Text::Regular);
    text_score.setPosition(x,y);  
}

class Apple_logo
{
public:
    Apple_logo();
    sf::Sprite s_logo_apple;
    sf::Texture t_logo_apple;
    int apple_touch = 0;
};
Apple_logo::Apple_logo()
{   
    t_logo_apple.loadFromFile("image_score.png");
    s_logo_apple.setTexture(t_logo_apple);
    s_logo_apple.setPosition(25, 1285);
}

class End_logo:public sf::Drawable
{
public:
    sf::Texture t_end_logo;
    sf::Sprite s_end_logo;
    bool show = false;
    void virtual draw(sf::RenderTarget&target, sf::RenderStates)const;
    End_logo();
};

void End_logo::draw(sf::RenderTarget& target, sf::RenderStates)const
{
    if (show)
    {
        target.draw(s_end_logo);
    }
}

End_logo::End_logo()
{ 
        t_end_logo.loadFromFile("logo_end.png");
        s_end_logo.setTexture(t_end_logo);
        s_end_logo.setPosition(120, 400); 
}




class Flags:public sf::Drawable
{
    sf::Texture t_eng_flag;
    sf::Texture t_rus_flag;
    sf::Texture t_language;
public:
    sf::Sprite s_language;
    sf::Sprite s_eng_flag;
    sf::Sprite s_rus_flag;
    bool show;
    virtual void draw(sf::RenderTarget& target,sf::RenderStates)const;
    Flags();
};

void Flags::draw(sf::RenderTarget& target,sf::RenderStates)const
{
    if (show)
    {
        target.draw(s_language);
        target.draw(s_eng_flag);
        target.draw(s_rus_flag);
    }
}

Flags::Flags():show(true)
{
    t_eng_flag.loadFromFile("english_flag.png");
    s_eng_flag.setTexture(t_eng_flag);
    s_eng_flag.setPosition(100,370);
    t_rus_flag.loadFromFile("russian_flag.png");
    s_rus_flag.setTexture(t_rus_flag);
    s_rus_flag.setPosition(390, 370);
    t_language.loadFromFile("language.png");
    s_language.setTexture(t_language);
    s_language.setPosition(68,260);
}

class The_choice_of_difficulty:public sf::Drawable
{
    sf::Texture t_easy_eng;
    sf::Texture t_easy_rus;
    sf::Texture t_medium_eng;
    sf::Texture t_medium_rus;
    sf::Texture t_hard_eng;
    sf::Texture t_hard_rus;  
    sf::Texture t_difficulty_rus;
    sf::Texture t_difficulty_eng;
public:
    virtual void draw(sf::RenderTarget&, sf::RenderStates)const;
    The_choice_of_difficulty();
    double difficulty_level = 0;
    bool show_rus = false;
    bool show_eng = true;
    bool activate_difficulty_buttons_eng = false;
    bool activate_difficulty_buttons_rus = false;
    sf::Sprite s_easy_eng;
    sf::Sprite s_easy_rus;
    sf::Sprite s_medium_eng;
    sf::Sprite s_medium_rus;
    sf::Sprite s_hard_eng;
    sf::Sprite s_hard_rus;
    sf::Sprite s_difficulty_rus;
    sf::Sprite s_difficulty_eng;
};

void The_choice_of_difficulty::draw(sf::RenderTarget& target, sf::RenderStates)const
{
      if (show_eng)
     {
        target.draw(s_easy_eng);
        target.draw(s_medium_eng);
        target.draw(s_hard_eng);
        target.draw(s_difficulty_eng);
     }
    if (show_rus)
    {
        target.draw(s_easy_rus);
        target.draw(s_medium_rus);
        target.draw(s_hard_rus);
        target.draw(s_difficulty_rus);
    }
}

The_choice_of_difficulty::The_choice_of_difficulty()
{
    t_easy_eng.loadFromFile("easy.png");
    t_easy_rus.loadFromFile("easy_rus.png");
    t_medium_eng.loadFromFile("medium.png");
    t_medium_rus.loadFromFile("medium_rus.png");
    t_hard_eng.loadFromFile("hard.png");
    t_hard_rus.loadFromFile("hard_rus.png");

    s_easy_eng.setTexture(t_easy_eng);
    s_easy_eng.setPosition(132,620);
    s_easy_rus.setTexture(t_easy_rus);
    s_easy_rus.setPosition(132, 620);

    s_medium_eng.setTexture(t_medium_eng);
    s_medium_eng.setPosition(130, 750);
    s_medium_rus.setTexture(t_medium_rus);
    s_medium_rus.setPosition(130, 750);

    s_hard_eng.setTexture(t_hard_eng);
    s_hard_eng.setPosition(130, 880);
    s_hard_rus.setTexture(t_hard_rus);
    s_hard_rus.setPosition(130, 880);

    t_difficulty_rus.loadFromFile("difficulty_rus.png");
    s_difficulty_rus.setTexture(t_difficulty_rus);
    s_difficulty_rus.setPosition(220,565);

    t_difficulty_eng.loadFromFile("difficulty_eng.png");
    s_difficulty_eng.setTexture(t_difficulty_eng);
    s_difficulty_eng.setPosition(220, 565);
}

class Sounds_of_game
{
public:
    sf::SoundBuffer buffer_pick_up;
    sf::SoundBuffer buffer_fail;
    sf::Sound sound_fail;
    sf::SoundBuffer buffer_end_game;
    sf::Sound sound_end_game;
    Sounds_of_game();
};

Sounds_of_game::Sounds_of_game()
{
    buffer_fail.loadFromFile("fail.ogg");
    sound_fail.setBuffer(buffer_fail);
    buffer_end_game.loadFromFile("the_end.ogg");
    sound_end_game.setBuffer(buffer_end_game);
}





int main()
{
    sf::RenderWindow window(sf::VideoMode(720, 1520), "snake");
	AutoHideNavBar();	
    window.setKeyRepeatEnabled(false);
    Head head_snake;
    Snake body_snake;
    Field field;   
    Fruit fruit;
    Buttons buttons;
    Text_score t(505,550);
    Text_score text_logo(83,1360);
    sf::Color color(240,238,26);
    Text_score score_end(150,600);
    score_end.text_score.setCharacterSize(60);
    score_end.text_score.setFillColor(color);
    Flags flags;
    Apple_logo logo;
    End_logo end_logo;
    The_choice_of_difficulty select_difficulty;
    Sounds_of_game sounds;
    char direction = 'q';
    bool stop_the_process = false;
    bool crutch_for_the_buttons = false;
    bool end = true;
    bool show_start_screen = false;
    bool the_end = false;
    sf::Clock simple_timer;
    sf::Time time = sf::seconds(0.1f);
    sf::Time t_vibrate = sf::microseconds(200000);
    std::list<std::shared_ptr<Snake>>body;
    std::list<Snake>::iterator it;
    
    body.push_back(std::shared_ptr<Snake>(new Snake(360,600)));
    body.push_back(std::shared_ptr<Snake>(new Snake(360,620)));
    body.push_back(std::shared_ptr<Snake>(new Snake(360,640)));
    body.push_back(std::shared_ptr<Snake>(new Snake(360,660)));
    std::list<std::shared_ptr<Purple_flower>>flower;
  
    for (int i = 0; i < 25; ++i)
    {
        flower.push_back(std::shared_ptr<Purple_flower>(new Purple_flower((int)random_numbers(3, 29) * 20,(int)random_numbers(4, 52) * 20)));
    }

    fruit.s_apple.setPosition((int)random_numbers(3, 29) * 20,(int)random_numbers(4, 52) * 20);

    
    for (auto it = flower.begin(); it != flower.end(); ++it)
    {
        if (fruit.s_apple.getGlobalBounds().intersects((*it)->s_purple_flower.getGlobalBounds()))
        {
            int x = (int)random_numbers(3, 29) * 20;
            int y = (int)random_numbers(4, 52) * 20;
            fruit.s_apple.setPosition(x, y);
        }
    }

    for (auto flower_it = flower.begin(); flower_it != flower.end(); ++flower_it)
    {
        for (auto body_it = body.begin(); body_it != body.end(); ++body_it)
        {
            if ((*flower_it)->s_purple_flower.getGlobalBounds().intersects((*body_it)->s_body.getGlobalBounds()))
            {
                (*flower_it)->s_purple_flower.setPosition((int)random_numbers(3, 29) * 20, (int)random_numbers(4, 52) * 20);
            }
        }
    }
    
    while (window.isOpen())
    {
        sf::Event event;
        time = simple_timer.getElapsedTime();

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::W)
                {
                    if (stop_the_process)
                    {
                        field.t_up_button.loadFromFile("up_yellow.png");
                        if (head_snake.i > 0 && (body.back()->y + 20) != (head_snake.y))
                        {
                            direction = 'w';
                        }

                        if (head_snake.i <= 0 && (body.back()->y + 20) != (head_snake.y))
                        {
                            buttons.show_button_play = false;
                            buttons.show = true;
                            stop_the_process = false;
                            crutch_for_the_buttons = true;
                            t.draw_text = true;
                            if (fruit.total < 1000) { sounds.sound_fail.play(); }
                            else
                            {
                                sounds.sound_end_game.play();
                                end_logo.show = true;
                                the_end = true;
                            }
                        }
                    }

                }

            }
            if (event.type == sf::Event::KeyReleased) { if (event.key.code == sf::Keyboard::W) { field.t_up_button.loadFromFile("up.png"); } }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::S)
                {
                    if (stop_the_process)
                    {
                        field.t_down_button.loadFromFile("down_yellow.png");
                        if (head_snake.i < 52 && (body.back()->y - 20) != (head_snake.y))
                        {
                            direction = 's';
                        }
                        if (head_snake.i >= 52 && (body.back()->y - 20) != (head_snake.y))
                        {
                            buttons.show_button_play = false;
                            buttons.show = true;
                            stop_the_process = false;
                            crutch_for_the_buttons = true;
                            t.draw_text = true;
                            if (fruit.total < 1000) { sounds.sound_fail.play(); }
                            else
                            {
                                sounds.sound_end_game.play();
                                end_logo.show = true;
                                the_end = true;
                            }
                        }
                    }
                }

            }

            if (event.type == sf::Event::KeyReleased) { if (event.key.code == sf::Keyboard::S) { field.t_down_button.loadFromFile("down.png"); } }

            if (event.type == sf::Event::KeyPressed)
            {

                if (event.key.code == sf::Keyboard::D)
                {
                    if (stop_the_process)
                    {
                        field.t_right_button.loadFromFile("right_yellow.png");
                        if (head_snake.j < 29 && (body.back()->x - 20) != (head_snake.x))
                        {
                            direction = 'd';
                        }
                        if (head_snake.j >= 29 && (body.back()->x - 20) != (head_snake.x))
                        {
                            buttons.show_button_play = false;
                            buttons.show = true;
                            stop_the_process = false;
                            crutch_for_the_buttons = true;
                            t.draw_text = true;
                            if (fruit.total < 1000) { sounds.sound_fail.play(); }
                            else
                            {
                                sounds.sound_end_game.play();
                                end_logo.show = true;
                                the_end = true;
                            }
                        }
                    }
                }

            }
            if (event.type == sf::Event::KeyReleased) { if (event.key.code == sf::Keyboard::D) { field.t_right_button.loadFromFile("right.png"); } }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::A)
                {
                    if (stop_the_process)
                    {
                        field.t_left_button.loadFromFile("left_yellow.png");
                        if (head_snake.j > 0 && (body.back()->x + 20) != (head_snake.x))
                        {
                            direction = 'a';
                        }
                        if (head_snake.j <= 0 && (body.back()->x + 20) != (head_snake.x))
                        {
                            buttons.show_button_play = false;
                            buttons.show = true;
                            stop_the_process = false;
                            crutch_for_the_buttons = true;
                            t.draw_text = true;
                            if (fruit.total < 1000) { sounds.sound_fail.play(); }
                            else
                            {
                                sounds.sound_end_game.play();
                                end_logo.show = true;
                                the_end = true;
                            }
                        }
                    }
                }

            }
            if (event.type == sf::Event::KeyReleased) { if (event.key.code == sf::Keyboard::A) { field.t_left_button.loadFromFile("left.png"); } }

            sf::Vector2i touch_pixel_position = sf::Touch::getPosition(0, window);
            sf::Vector2f pixel_position = window.mapPixelToCoords(touch_pixel_position);

            if (event.type == sf::Event::TouchBegan)
            {
                if (stop_the_process)
                {
                    if (field.s_up_button.getGlobalBounds().contains(pixel_position))
                    {
                        if (head_snake.i > 0 && (body.back()->y + 20) != (head_snake.y))
                        {
                            direction = 'w'; field.t_up_button.loadFromFile("up_yellow.png");
                        }
                        if (head_snake.i <= 0 && (body.back()->y + 20) != (head_snake.y))
                        {
                            buttons.show_button_play = false;
                            buttons.show = true;
                            stop_the_process = false;
                            crutch_for_the_buttons = true;
                            t.draw_text = true;
                            if (fruit.total < 1000) { sounds.sound_fail.play(); }
                            else
                            {
                                sounds.sound_end_game.play();
                                end_logo.show = true;
                                the_end = true;
                            }
                        }
                    }

                    if (field.s_down_button.getGlobalBounds().contains(pixel_position))
                    {
                        if (head_snake.i < 52 && (body.back()->y - 20) != (head_snake.y))
                        {
                            direction = 's'; field.t_down_button.loadFromFile("down_yellow.png");
                        }
                        if (head_snake.i >= 52 && (body.back()->y - 20) != (head_snake.y))
                        {
                            buttons.show_button_play = false;
                            buttons.show = true;
                            stop_the_process = false;
                            crutch_for_the_buttons = true;
                            t.draw_text = true;
                            if (fruit.total < 1000) { sounds.sound_fail.play(); }
                            else
                            {
                                sounds.sound_end_game.play();
                                end_logo.show = true;
                                the_end = true;
                            }
                        }

                    }

                    if (field.s_right_button.getGlobalBounds().contains(pixel_position))
                    {
                        if (head_snake.j < 29 && (body.back()->x - 20) != (head_snake.x))
                        {
                            { direction = 'd'; field.t_right_button.loadFromFile("right_yellow.png"); }
                        }
                        if (head_snake.j >= 29 && (body.back()->x - 20) != (head_snake.x))
                        {
                            buttons.show_button_play = false;
                            buttons.show = true;
                            stop_the_process = false;
                            crutch_for_the_buttons = true;
                            t.draw_text = true;
                            if (fruit.total < 1000) { sounds.sound_fail.play(); }
                            else
                            {
                                sounds.sound_end_game.play();
                                end_logo.show = true;
                                the_end = true;
                            }
                        }
                    }

                    if (field.s_left_button.getGlobalBounds().contains(pixel_position))
                    {
                        if (head_snake.j > 0 && (body.back()->x + 20) != (head_snake.x))
                        {
                            { direction = 'a'; field.t_left_button.loadFromFile("left_yellow.png"); }
                        }
                        if (head_snake.j <= 0 && (body.back()->x + 20) != (head_snake.x))
                        {
                            buttons.show_button_play = false;
                            buttons.show = true;
                            stop_the_process = false;
                            crutch_for_the_buttons = true;
                            t.draw_text = true;
                            if (fruit.total < 1000) { sounds.sound_fail.play(); }
                            else
                            {
                                sounds.sound_end_game.play();
                                end_logo.show = true;
                                the_end = true;
                            }
                        }

                    }
                }
            }

            if (crutch_for_the_buttons)
            {
                if (buttons.s_button_three.getGlobalBounds().contains(pixel_position))
                {
                    buttons.show_button_play = true;
                    buttons.show = false;
                    end_logo.show = false;
                    end = true;
                    stop_the_process = true;
                    crutch_for_the_buttons = false;
                    the_end = false;
                    logo.apple_touch = 0;
                    flower.clear();
                    for (int i = 0; i < 25; ++i) { flower.push_back(std::shared_ptr<Purple_flower>(new Purple_flower((int)random_numbers(3, 29) * 20, (int)random_numbers(4, 52) * 20))); }
                    fruit.s_apple.setPosition((int)random_numbers(3, 29) * 20, (int)random_numbers(4, 52) * 20);
                    simple_timer.restart();
                    direction = 'q';
                    head_snake.x = 360;
                    head_snake.y = 680;
                    head_snake.s_head.setPosition(360, 680);
                    head_snake.j = 15;
                    head_snake.i = 30;
                    body.clear();
                    body.push_back(std::shared_ptr<Snake>(new Snake(360, 600)));
                    body.push_back(std::shared_ptr<Snake>(new Snake(360, 620)));
                    body.push_back(std::shared_ptr<Snake>(new Snake(360, 640)));
                    body.push_back(std::shared_ptr<Snake>(new Snake(360, 660)));
                    t.draw_text = false;
                    fruit.total = 0;
                    text_logo.text_score.setPosition(83, 1360);
                    t.str = std::to_string(fruit.total);
                    t.text_score.setString(t.str);
                    text_logo.text_score.setString(t.str);
                    score_end.text_score.setString(t.str);
                }
            }

            if (event.type == sf::Event::TouchEnded)
            {
                if (!field.s_up_button.getGlobalBounds().contains(pixel_position)) { field.t_up_button.loadFromFile("up.png"); }
                if (!field.s_down_button.getGlobalBounds().contains(pixel_position)) { field.t_down_button.loadFromFile("down.png"); }
                if (!field.s_right_button.getGlobalBounds().contains(pixel_position)) { field.t_right_button.loadFromFile("right.png"); }
                if (!field.s_left_button.getGlobalBounds().contains(pixel_position)) { field.t_left_button.loadFromFile("left.png"); }
            }

            sf::Vector2i mouse_pixel_position = sf::Mouse::getPosition(window);
            sf::Vector2f pos = window.mapPixelToCoords(mouse_pixel_position);

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.key.code == sf::Mouse::Left)
                {
                    if (crutch_for_the_buttons)
                    {
                        if (buttons.s_button_three.getGlobalBounds().contains(pos))
                        {
                            buttons.show_button_play = true;
                            buttons.show = false;
                            end_logo.show = false;
                            end = true;
                            stop_the_process = true;
                            crutch_for_the_buttons = false;
                            the_end = false;
                            logo.apple_touch = 0;
                            flower.clear();
                            for (int i = 0; i < 25; ++i) { flower.push_back(std::shared_ptr<Purple_flower>(new Purple_flower((int)random_numbers(3, 29) * 20, (int)random_numbers(4, 52) * 20))); }
                            fruit.s_apple.setPosition((int)random_numbers(3, 29) * 20, (int)random_numbers(4, 52) * 20);
                            simple_timer.restart();
                            direction = 'q';
                            head_snake.x = 360;
                            head_snake.y = 680;
                            head_snake.s_head.setPosition(360, 680);
                            head_snake.j = 15;
                            head_snake.i = 30;
                            body.clear();
                            body.push_back(std::shared_ptr<Snake>(new Snake(360, 600)));
                            body.push_back(std::shared_ptr<Snake>(new Snake(360, 620)));
                            body.push_back(std::shared_ptr<Snake>(new Snake(360, 640)));
                            body.push_back(std::shared_ptr<Snake>(new Snake(360, 660)));
                            t.draw_text = false;
                            fruit.total = 0;
                            text_logo.text_score.setPosition(83, 1360);
                            t.str = std::to_string(fruit.total);
                            t.text_score.setString(t.str);
                            text_logo.text_score.setString(t.str);
                            score_end.text_score.setString(t.str);                        
                        }
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.key.code == sf::Mouse::Left)
                {
                    if (flags.show)
                    {
                        if (flags.s_rus_flag.getGlobalBounds().contains(pos))
                        {
                            buttons.t_button_one.loadFromFile("button1.png");
                            buttons.t_button_two.loadFromFile("button2.png");
                            buttons.t_button_three.loadFromFile("button3.png");
                            flags.show = false;
                            select_difficulty.show_rus = true; 
                            select_difficulty.show_eng = false;
                            select_difficulty.activate_difficulty_buttons_rus = true;
                        }
                        if (flags.s_eng_flag.getGlobalBounds().contains(pos))
                        {
                            buttons.t_button_one.loadFromFile("button1_eng.png");
                            buttons.t_button_two.loadFromFile("button2_eng.png");
                            buttons.t_button_three.loadFromFile("button3_eng.png");
                            flags.show = false;
                            select_difficulty.show_eng = true;
                            select_difficulty.show_rus = false;
                            select_difficulty.activate_difficulty_buttons_rus = true;
                        }


                    }
                }
            }




            if (buttons.show_button_play)
            {
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.key.code == sf::Mouse::Left)
                    {
                        if (buttons.s_button_play.getGlobalBounds().contains(pos))
                        {
                            if (buttons.check)
                            {
                                buttons.t_button_play.loadFromFile("stop_button.png");
                                buttons.check = false;
                                stop_the_process = false;
                            }
                            else
                            {
                                buttons.t_button_play.loadFromFile("play_button.png");
                                buttons.check = true;
                                stop_the_process = true;
                            }
                        }
                    }
                }
            }

            if (buttons.show_button_play)
            {
                if (event.type == sf::Event::TouchBegan)
                {
                    if (buttons.s_button_play.getGlobalBounds().contains(pixel_position))
                    {
                        if (buttons.check)
                        {
                            buttons.t_button_play.loadFromFile("stop_button.png");
                            buttons.check = false;
                            stop_the_process = false;
                        }
                        else
                        {
                            buttons.t_button_play.loadFromFile("play_button.png");
                            buttons.check = true;
                            stop_the_process = true;
                        }
                    }
                }
            }

            if (event.type == sf::Event::TouchBegan)
            {
                if (flags.show)
                {
                    if (flags.s_rus_flag.getGlobalBounds().contains(pixel_position))
                    {
                        buttons.t_button_one.loadFromFile("button1.png");
                        buttons.t_button_two.loadFromFile("button2.png");
                        buttons.t_button_three.loadFromFile("button3.png");
                        flags.show = false;
                        select_difficulty.show_rus = true;
                        select_difficulty.show_eng = false;
                        select_difficulty.activate_difficulty_buttons_rus = true;
                    }

                }

            }
            if (event.type == sf::Event::TouchBegan)
            {
                if (flags.show)
                {
                    if (flags.s_eng_flag.getGlobalBounds().contains(pixel_position))
                    {
                        buttons.t_button_one.loadFromFile("button1_eng.png");
                        buttons.t_button_two.loadFromFile("button2_eng.png");
                        buttons.t_button_three.loadFromFile("button3_eng.png");
                        flags.show = false;
                        select_difficulty.show_eng = true;
                        select_difficulty.show_rus = false;                        
                        select_difficulty.activate_difficulty_buttons_eng = true;
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.key.code == sf::Mouse::Left)
                {
                    if (logo.s_logo_apple.getGlobalBounds().contains(pos))
                    {
                        logo.apple_touch++;
                        if (logo.apple_touch == 30)
                        {
                            select_difficulty.show_rus = false;
                            select_difficulty.show_eng = false;
                            sounds.sound_end_game.play();
                            stop_the_process = false;
                            end_logo.show = true;
                            the_end = true;
                        }
                    }
                }
            }

            if (event.type == sf::Event::TouchBegan)
            {
                if (logo.s_logo_apple.getGlobalBounds().contains(pixel_position))
                {
                    logo.apple_touch++;
                    if (logo.apple_touch == 30)
                    {
                        select_difficulty.show_rus = false;
                        select_difficulty.show_eng = false;
                        sounds.sound_end_game.play();
                        stop_the_process = false;
                        end_logo.show = true;
                        the_end = true;
                    }
                }
            }

            if (select_difficulty.activate_difficulty_buttons_eng)
            {
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.key.code == sf::Mouse::Left)
                    {
                        if (select_difficulty.s_easy_eng.getGlobalBounds().contains(pos))
                        {
                            select_difficulty.activate_difficulty_buttons_eng = false;
                            select_difficulty.show_eng = false;
                            stop_the_process = true;
                            select_difficulty.difficulty_level = 0.6;
                            buttons.show_button_play = true;
                            show_start_screen = true;
                        }
                        else   if (select_difficulty.s_medium_eng.getGlobalBounds().contains(pos))
                        {
                            select_difficulty.activate_difficulty_buttons_eng = false;
                            select_difficulty.show_eng = false;
                            stop_the_process = true;
                            select_difficulty.difficulty_level = 0.4;
                            buttons.show_button_play = true;
                            show_start_screen = true;
                        }
                        else    if (select_difficulty.s_hard_eng.getGlobalBounds().contains(pos))
                        {
                            select_difficulty.activate_difficulty_buttons_eng = false;
                            select_difficulty.show_eng = false;
                            stop_the_process = true;
                            select_difficulty.difficulty_level = 0.2;
                            buttons.show_button_play = true;
                            show_start_screen = true;  
                        }

                    }
                }
            }

            if (select_difficulty.activate_difficulty_buttons_eng)
            {
                if (event.type == sf::Event::TouchBegan)
                {
                    if (select_difficulty.s_easy_eng.getGlobalBounds().contains(pixel_position))
                    {
                        select_difficulty.show_eng = false;
                        select_difficulty.show_rus = false;
                        select_difficulty.activate_difficulty_buttons_eng = false;
                        stop_the_process = true;
                        select_difficulty.difficulty_level = 0.6;
                        buttons.show_button_play = true;
                        show_start_screen = true;
                    }
                    else   if (select_difficulty.s_medium_eng.getGlobalBounds().contains(pixel_position))
                    {
                        select_difficulty.show_eng = false;
                        select_difficulty.show_rus = false;
                        select_difficulty.activate_difficulty_buttons_eng = false;
                        stop_the_process = true;
                        select_difficulty.difficulty_level = 0.4;
                        buttons.show_button_play = true;
                        show_start_screen = true;
                    }
                    else    if (select_difficulty.s_hard_eng.getGlobalBounds().contains(pixel_position))
                    {
                        select_difficulty.show_eng = false;
                        select_difficulty.show_rus = false;
                        select_difficulty.activate_difficulty_buttons_eng = false;
                        stop_the_process = true;
                        select_difficulty.difficulty_level = 0.2;
                        buttons.show_button_play = true;
                        show_start_screen = true;
                    }

                }
            }

            if (select_difficulty.activate_difficulty_buttons_rus)
            {
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.key.code == sf::Mouse::Left)
                    {
                        if (select_difficulty.s_easy_rus.getGlobalBounds().contains(pos))
                        {
                            select_difficulty.activate_difficulty_buttons_rus = false;
                            select_difficulty.show_rus = false;
                            select_difficulty.show_eng = false;
                            stop_the_process = true;
                            select_difficulty.difficulty_level = 0.6;
                            buttons.show_button_play = true;
                            show_start_screen = true;
                        }
                        else   if (select_difficulty.s_medium_rus.getGlobalBounds().contains(pos))
                        {
                            select_difficulty.activate_difficulty_buttons_rus = false;
                            select_difficulty.show_rus = false;
                            select_difficulty.show_eng = false;
                            stop_the_process = true;
                            select_difficulty.difficulty_level = 0.4;
                            buttons.show_button_play = true;
                            show_start_screen = true;
                        }
                        else    if (select_difficulty.s_hard_rus.getGlobalBounds().contains(pos))
                        {
                            select_difficulty.activate_difficulty_buttons_rus = false;
                            select_difficulty.show_rus = false;
                            select_difficulty.show_eng = false;
                            stop_the_process = true;
                            select_difficulty.difficulty_level = 0.2;
                            buttons.show_button_play = true;
                            show_start_screen = true;
                        }

                    }
                }
            }


            if (select_difficulty.activate_difficulty_buttons_rus)
            {
                if (event.type == sf::Event::TouchBegan)
                {
                    if (select_difficulty.s_easy_rus.getGlobalBounds().contains(pixel_position))
                    {
                        select_difficulty.activate_difficulty_buttons_rus = false;
                        select_difficulty.show_eng = false;
                        select_difficulty.show_rus = false;
                        stop_the_process = true;
                        select_difficulty.difficulty_level = 0.6;
                        buttons.show_button_play = true;
                        show_start_screen = true;
                    }
                    else   if (select_difficulty.s_medium_rus.getGlobalBounds().contains(pixel_position))
                    {
                        select_difficulty.activate_difficulty_buttons_rus = false;
                        select_difficulty.show_eng = false;
                        select_difficulty.show_rus = false;
                        stop_the_process = true;
                        select_difficulty.difficulty_level = 0.4;
                        buttons.show_button_play = true;
                        show_start_screen = true;
                    }
                    else    if (select_difficulty.s_hard_rus.getGlobalBounds().contains(pixel_position))
                    {
                        select_difficulty.activate_difficulty_buttons_rus = false;
                        select_difficulty.show_eng = false;
                        select_difficulty.show_rus = false;
                        stop_the_process = true;
                        select_difficulty.difficulty_level = 0.2;
                        buttons.show_button_play = true;
                        show_start_screen = true;
                    }

                }
            }

            if (event.type == sf::Event::Closed)
                window.close();
        }




        switch (direction)
        {
        case 'w':
            if (time.asSeconds() > select_difficulty.difficulty_level)
            {
                if (stop_the_process)
                {
                    if (head_snake.i > 0 && (body.back()->y + 20) != (head_snake.y))
                    {
                        head_snake.i--;
                        body.push_back(std::shared_ptr<Snake>(new Snake(head_snake.x, head_snake.y)));
                        head_snake.s_head.setPosition(head_snake.x, head_snake.y -= 20);
                        body.pop_front();
                    }
                    else
                    {
                        buttons.show_button_play = false;
                        buttons.show = true;
                        stop_the_process = false;
                        crutch_for_the_buttons = true;
                        t.draw_text = true;
                        if (fruit.total < 1000) { sounds.sound_fail.play(); }
                        else
                        {
                            sounds.sound_end_game.play();
                            end_logo.show = true;
                            the_end = true;
                        }
                    }
                }
                simple_timer.restart();
            }break;


        case 's':
            if (time.asSeconds() > select_difficulty.difficulty_level)
            {
                if (stop_the_process)
                {
                    if (head_snake.i < 52 && (body.back()->y - 20) != (head_snake.y))
                    {
                        head_snake.i++;
                        body.push_back(std::shared_ptr<Snake>(new Snake(head_snake.x, head_snake.y)));
                        head_snake.s_head.setPosition(head_snake.x, head_snake.y += 20);
                        body.pop_front();
                    }
                    else
                    {
                        buttons.show_button_play = false;
                        buttons.show = true;
                        stop_the_process = false;
                        crutch_for_the_buttons = true;
                        t.draw_text = true;
                        if (fruit.total < 1000) { sounds.sound_fail.play(); }
                        else
                        {
                            sounds.sound_end_game.play();
                            end_logo.show = true;
                            the_end = true;
                        }
                    }
                }
                simple_timer.restart();

            }break;
        case'a':
            if (time.asSeconds() > select_difficulty.difficulty_level)
            {
                if (stop_the_process)
                {
                    if (head_snake.j > 0 && (body.back()->x + 20) != (head_snake.x))
                    {
                        head_snake.j--;
                        body.push_back(std::shared_ptr<Snake>(new Snake(head_snake.x, head_snake.y)));
                        head_snake.s_head.setPosition(head_snake.x -= 20, head_snake.y);
                        body.pop_front();
                    }
                    else
                    {
                        buttons.show_button_play = false;
                        buttons.show = true;
                        stop_the_process = false;
                        crutch_for_the_buttons = true;
                        t.draw_text = true;
                        if (fruit.total < 1000) { sounds.sound_fail.play(); }
                        else
                        {
                            sounds.sound_end_game.play();
                            end_logo.show = true;
                            the_end = true;
                        }
                    }
                }
                simple_timer.restart();

            }break;
        case'd':
            if (time.asSeconds() > select_difficulty.difficulty_level)
            {
                if (stop_the_process)
                {
                    if ((head_snake.j < 29) && (body.back()->x - 20) != head_snake.x)
                    {
                        head_snake.j++;
                        body.push_back(std::shared_ptr<Snake>(new Snake(head_snake.x, head_snake.y)));
                        head_snake.s_head.setPosition(head_snake.x += 20, head_snake.y);
                        body.pop_front();
                    }
                    else
                    {
                        buttons.show_button_play = false;
                        buttons.show = true;
                        stop_the_process = false;
                        crutch_for_the_buttons = true;
                        t.draw_text = true;
                        if (fruit.total < 1000) { sounds.sound_fail.play(); }
                        else
                        {
                            sounds.sound_end_game.play();
                            end_logo.show = true;
                            the_end = true;
                        }
                    }
                }
                simple_timer.restart();

            }break;
        }

        sf::FloatRect head_coordinats = head_snake.s_head.getGlobalBounds();
        if (fruit.s_apple.getGlobalBounds().intersects(head_coordinats))
        {
            body.push_back(std::shared_ptr<Snake>(new Snake(body.back()->x, body.back()->y)));
            int x = (int)random_numbers(3, 29) * 20;
            int y = (int)random_numbers(4, 52) * 20;
            fruit.total++;
            vibrate(t_vibrate);
            fruit.s_apple.setPosition(x, y);
            sf::Vector2f position = head_snake.s_head.getPosition();
            if ((position.x == x) && (position.y == y))
            {
                x = (int)random_numbers(3, 29) * 20;
                y = (int)random_numbers(4, 52) * 20;
                fruit.s_apple.setPosition(x, y);
            }

            if (fruit.total == 10) { text_logo.text_score.setPosition(78, 1360); }
            if (fruit.total == 100) { text_logo.text_score.setPosition(65, 1360); }
            if (fruit.total == 1000) { text_logo.text_score.setPosition(52, 1360); }
            if (fruit.total == 10000) { text_logo.text_score.setPosition(39, 1360); }

            t.str = std::to_string(fruit.total);
            t.text_score.setString(t.str);
            text_logo.text_score.setString(t.str);
            score_end.text_score.setString(t.str);
        }

        for (auto it = body.begin(); it != body.end(); ++it)
        {
            if (fruit.s_apple.getGlobalBounds().intersects((*it)->s_body.getGlobalBounds()))
            {
                int x = (int)random_numbers(3, 29) * 20;
                int y = (int)random_numbers(4, 52) * 20;
                fruit.s_apple.setPosition(x, y);
            }
        }

        for (auto it = flower.begin(); it != flower.end(); ++it)
        {
            if (fruit.s_apple.getGlobalBounds().intersects((*it)->s_purple_flower.getGlobalBounds()))
            {
                int x = (int)random_numbers(3, 29) * 20;
                int y = (int)random_numbers(4, 52) * 20;
                fruit.s_apple.setPosition(x, y);
            }
        }

        for (auto flower_it = flower.begin(); flower_it != flower.end(); ++flower_it)
        {
            for (auto body_it = body.begin(); body_it != body.end(); ++body_it)
            {
                if ((*flower_it)->s_purple_flower.getGlobalBounds().intersects((*body_it)->s_body.getGlobalBounds()))
                {
                    (*flower_it)->s_purple_flower.setPosition((int)random_numbers(3, 29) * 20, (int)random_numbers(4, 52) * 20);
                }
            }
        }

        if (end)
        {
            for (auto it = body.begin(); it != body.end(); ++it)
            {
                if ((*it)->s_body.getGlobalBounds().intersects(head_coordinats))
                {
                    buttons.show_button_play = false;
                    buttons.show = true;
                    stop_the_process = false;
                    crutch_for_the_buttons = true;
                    t.draw_text = true;
                    end = false;

                    if (fruit.total < 1000) { sounds.sound_fail.play(); }
                    else
                    {  
                        sounds.sound_end_game.play();
                        end_logo.show = true;
                        the_end = true;
                    }
                }

            }
        }

        window.clear();
        window.draw(field);

        if (show_start_screen) 
        {
            if (!end_logo.show)
            {
                if (!flags.show)
                {
                    window.draw(fruit.s_apple);
                        window.draw(head_snake.s_head);
                        for (auto it = flower.begin(); it != flower.end(); ++it) { window.draw((*it)->s_purple_flower); }
                    for (auto it = body.begin(); it != body.end(); ++it) { window.draw((*it)->s_body); }
                }

            }
        }
        for (auto it = flower.begin(); it != flower.end();)
        {
            if ((*it)->s_purple_flower.getGlobalBounds().intersects(head_coordinats))
            {
                buttons.show_button_play = false;
                buttons.show = true;
                stop_the_process = false;
                crutch_for_the_buttons = true;
                it = flower.erase(it);
                t.draw_text = true;  
                if (fruit.total < 1000) { sounds.sound_fail.play(); }
                else
                {                    
                    sounds.sound_end_game.play();
                    end_logo.show = true;
                    the_end = true;
                }
            }
            else
            {
                it++;
            }
        }
        if (!end_logo.show)
        {
            window.draw(buttons);

            if (t.draw_text)
            {
                window.draw(t.text_score);
            }
        }
        window.draw(logo.s_logo_apple);
        window.draw(text_logo.text_score);
        if (the_end)
        {
            window.draw(score_end.text_score);
        }
        AutoHideNavBar();
        window.draw(flags);
        window.draw(end_logo);
        window.draw(select_difficulty);
        window.display();
    }
    
    return 0;
}

double random_numbers(double a, double b)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double>dist(a, b);
    return dist(mt);
}























