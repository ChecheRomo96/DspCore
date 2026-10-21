function plot_signal(csvPath, imagePath)
%PLOT_SIGNAL Plot a DspCore desktop-example CSV in time and frequency.
%   PLOT_SIGNAL("DspCoreExampleOutput/fir_moving_average.csv") opens a
%   two-panel time/frequency view. Supply a second path to write a PNG.

if nargin < 1
    error("Usage: plot_signal(csvPath, [imagePath])")
end

options = detectImportOptions(csvPath, "CommentStyle", "#");
signal = readtable(csvPath, options);
sampleRate = 1 / mean(diff(signal.time_seconds));
channels = setdiff(signal.Properties.VariableNames, {"sample", "time_seconds"}, "stable");
visible = signal.time_seconds <= signal.time_seconds(1) + 0.05;
contents = fileread(csvPath);
plotMode = regexp(contents, '# plot_mode=([^\r\n]+)', 'tokens', 'once');
if isempty(plotMode)
    plotMode = "time_and_spectrum";
else
    plotMode = string(plotMode{1});
end

figure("Name", "DspCore signal", "Color", "white");
if plotMode == "time_and_spectrum"
    tiledlayout(2, 1);
    nexttile;
elseif plotMode ~= "time"
    error("Unsupported DspCore plot mode: " + plotMode)
end
hold on;
for index = 1:numel(channels)
    plot(signal.time_seconds(visible), signal.(channels{index})(visible), "DisplayName", channels{index});
end
hold off;
grid on;
xlabel("time (s)");
ylabel("amplitude");
legend("Location", "best");
title("Signal response");

if plotMode == "time_and_spectrum"
    nexttile;
    count = height(signal);
    window = 0.5 - 0.5 * cos(2 * pi * (0:count-1)' / (count - 1));
    frequencies = sampleRate * (0:floor(count / 2))' / count;
    hold on;
    for index = 1:numel(channels)
        spectrum = abs(fft(signal.(channels{index}) .* window));
        spectrum = spectrum(1:floor(count / 2) + 1);
        magnitude = 20 * log10(max(spectrum / max(spectrum), 1e-8));
        plot(frequencies, magnitude, "DisplayName", channels{index});
    end
    hold off;
    grid on;
    xlim([0 sampleRate / 2]);
    ylim([-80 3]);
    xlabel("frequency (Hz)");
    ylabel("magnitude (dB, normalized)");
    legend("Location", "best");
    title("Magnitude spectrum");
end

if nargin >= 2
    exportgraphics(gcf, imagePath, "Resolution", 160);
end
end
