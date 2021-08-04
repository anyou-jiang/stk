// crtsine.cpp STK tutorial program
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "SineWave.h"
#include "RtAudio.h"

using namespace::std;

using namespace stk;
// This tick() function handles sample computation only.  It will be
// called automatically when the system needs a new buffer of audio
// samples.
int tick(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
    double streamTime, RtAudioStreamStatus status, void* dataPointer)
{
    SineWave* sine = (SineWave*)dataPointer;
    std::vector<std::shared_ptr<SineWave>>* sines = (std::vector<std::shared_ptr<SineWave>>*)dataPointer;
    register StkFloat* samples = (StkFloat*)outputBuffer;
    for (unsigned int i = 0; i < nBufferFrames; i++)
    {
        *samples = 0.0;
        const unsigned n_sine = sines->size();
        for (unsigned sine_i = 0; sine_i < n_sine; sine_i++)
        {
            *samples = *samples + 0.1 * (*sines)[sine_i]->tick();
        }
        samples++;
    }
    return 0;
}
int main()
{
    //Read parameters from a file
    fstream input_file;
    vector<double> frequencies;
    vector<double> amplitudes;
    input_file.open("input.txt", ios::in);
    unsigned input_status = 0;
    if (input_file.is_open()) {
        string frq;
        string amp;
        std::getline(input_file, frq);
        std::getline(input_file, amp);
        std::cout << "frequencies = " << frq << std::endl;
        std::cout << "amplitudes = " << amp << std::endl;

        
        stringstream frq_stream(frq);  // split into double number
        while (frq_stream.good()) {
            string substr;
            std::getline(frq_stream, substr, ','); 
            frequencies.push_back(stod(substr));
        }

        
        stringstream amp_stream(amp);  // split into double number
        while (amp_stream.good()) {
            string substr;
            std::getline(amp_stream, substr, ',');
            amplitudes.push_back(stod(substr));
        }

        if (frequencies.size() != amplitudes.size()) {
            input_status = 1;
            std::cout << "the number of the frequencies should be equal to the number of amplitudes in the input file." << std::endl;
        }
    }
    else
    {
        input_status = 1;
        std::cout << "could not open input.txt in the application folder." << std::endl;
    }

    if (input_status != 0)
    {
        return -1;
    }

    const unsigned n_sine = frequencies.size();
    std::vector<std::shared_ptr<SineWave>> sines;
    for (unsigned sine_i = 0; sine_i < n_sine; sine_i++)
    {
        std::shared_ptr<SineWave> sine = std::shared_ptr<SineWave>(new SineWave());
        sine->setFrequency(frequencies[sine_i]);
        sines.push_back(sine);
    }

    // Set the global sample rate
    Stk::setSampleRate(44100.0);

    RtAudio dac;
    // Figure out how many bytes in an StkFloat and setup the RtAudio stream.
    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac.getDefaultOutputDevice();
    parameters.nChannels = 1;
    RtAudioFormat format = (sizeof(StkFloat) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
    unsigned int bufferFrames = RT_BUFFER_SIZE;
    try {
        //dac.openStream(&parameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &tick, (void*)&sine);
        dac.openStream(&parameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &tick, (void*)&sines);
    }
    catch (RtAudioError& error) {
        error.printMessage();
        goto cleanup;
    }

    try {
        dac.startStream();
    }
    catch (RtAudioError& error) {
        error.printMessage();
        goto cleanup;
    }
    // Block waiting here.
    char keyhit;
    std::cout << "\nPlaying ... press <enter> to quit.\n";
    std::cin.get(keyhit);
    // Shut down the output stream.
    try {
        dac.closeStream();
    }
    catch (RtAudioError& error) {
        error.printMessage();
    }
cleanup:
    return 0;
}