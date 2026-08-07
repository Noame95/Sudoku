console.log("sounds.js loaded");
const AudioContextClass = window.AudioContext || window.webkitAudioContext;
const audioContext = new AudioContextClass();

function playTone(frequency, duration, type = "sine", volume = 0.15, delay = 0) {
    const oscillator = audioContext.createOscillator();
    const gain = audioContext.createGain();
    const startTime = audioContext.currentTime + delay;
    oscillator.type = type;
    oscillator.frequency.setValueAtTime(frequency, startTime);
    gain.gain.setValueAtTime(volume, startTime);
    gain.gain.exponentialRampToValueAtTime(0.001, startTime + duration);
    oscillator.connect(gain);
    gain.connect(audioContext.destination);
    oscillator.start(startTime);
    oscillator.stop(startTime + duration);
}

function playSuccessSound() {
    playTone(660, 0.12);
    playTone(880, 0.15, "sine", 0.15, 0.09);
}

function playErrorSound() {
    playTone(180, 0.2, "sawtooth", 0.1);
}

function playHintSound() {
    playTone(440, 0.1);
    playTone(660, 0.12, "triangle", 0.12, 0.08);
}

function playVictorySound() {
    playTone(523, 0.14, "sine", 0.15, 0);
    playTone(659, 0.14, "sine", 0.15, 0.12);
    playTone(784, 0.18, "sine", 0.15, 0.24);
    playTone(1046, 0.3, "sine", 0.18, 0.38);
}