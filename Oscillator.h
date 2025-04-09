/*
  ==============================================================================

    oscillateur.h
    Created: 4 Feb 2025 4:51:14pm
    Author:  matis

  ==============================================================================
*/

#pragma once

//#include "PluginProcessor.h"
//#include "PluginEditor.h"
//#include "tremolo.h"

class Oscillator
{
private:
    float previous_modulo = 0.0f;
    float randomValue = 0.0f;

public:
    float inc;
    float modulo_counter;
    float amplitude;
    int waveform = 0; //Initialisation de la variable waveform pour la sélection de la forme d'onde


    void reset()
    {
        modulo_counter = 0.0f;
    }

    void setAmplitude(float amplitude)
    {
        this->amplitude = amplitude;
    }

    float nextSample()
    {
        float output = 0.0f;

        modulo_counter += inc;
        if (modulo_counter >= 1.0f)
            modulo_counter -= 1.0f;

        //Création des différentes formes d'ondes
        //Les différentes formes d'ondes sont associées à un numéro pour pouvoir les sélectionner
        if (waveform == 0) { //Onde sinus
            output = std::sin(juce::MathConstants<float>::twoPi * modulo_counter);
        }

        //Définissons l'onde carrée comme étant centrée en 0 et alternant entre -1 et 1
        // Si le sinus est supérieur à O alors l'onde carrée est égal à 1 et sinon l'onde carrée vaut -1
        else if (waveform == 1) { //Onde carré
            output = (std::sin(juce::MathConstants<float>::twoPi * modulo_counter) > 0.0f) ? 1.0f : -1.0f;
        }

        //Le modulo_counter va de 0 à 1 en s'incrémentant à chaque nouveau sample. Quand il atteint 1
        //il revient à 0. La fonction floor permet d'arrondir à l'entier inférieur ou égal à la valeur.
        //La soustraction permet d'avoir des valeurs comprises entre -0.5 et 0.5. Enfin la multiplication 
        //par 2 permet d'avoir des valeurs qui vont de -1 à 1. Ce qui va créer l'onde dent de scie
        else if (waveform == 2) { //Onde dent de scie
            output = 2.0f * (modulo_counter - std::floor(modulo_counter + 0.5f));
        }

        //Pour l'onde triangle, on reprend la formule de l'onde en dent de scie que l'on va 
        //où l'on va prendre la valeur absolue pour avoir une forme de V et ensuite on va centrée l'onde en 0
        //entre -1 et 1. Car avec la valeur absolue on avait une onde entre 0 et 0.5.
        else if (waveform == 3) { //Onde triangle
            output = 2.0f * std::abs(2.0f * (modulo_counter - std::floor(modulo_counter + 0.5f))) - 1.0f;
        }

        //Nouvelle valeur aléatoire à chaque cycle complet c'est-à-dire quand le modulo_counter revient à 0, une fois 
        //cette valeur sélectionnée, on la garde constante pendant tout le cycle.
        else if (waveform == 4) { //Onde aléatoire
            // LFO aléatoire : nouvelle valeur à chaque cycle
            if (modulo_counter < previous_modulo)
                randomValue = juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f;

            output = randomValue;

            previous_modulo = modulo_counter;
        }

        return output * amplitude;

    }
};
