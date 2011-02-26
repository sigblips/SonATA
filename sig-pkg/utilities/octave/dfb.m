% Octave/Matlab program
% DFB simulation
% System variables
spectra = 1024;					% of output spectra
filterFile = "LS8c10f";			% name of DFB filter file
fftLen = 8;						% length of FFT
foldings = 10;					% % of foldings in window
L = fftLen * foldings;			% total length of filter
oversampling = 0.25;			% amount of oversampling
overlap = fftLen * oversampling;
R = fftLen - overlap;			% samples to advance between spectra
iter = spectra;
% get the frequencies to use
f = sprintf("freq 1 (0-%d): ", fftLen);
freq1 = input(f);
f = sprintf("freq 2 (0-%d): ", fftLen);
freq2 = input(f);
% compute the total number of samples
samples = L + (spectra - 1) * R; % # of time samples
x = zeros(1, samples - 1);
i = 0:samples-1;
x(i+1) = complex(cos(freq1 * 2* pi * (i) / fftLen), sin(freq1 * 2* pi * (i) / fftLen));
if (freq2 >= 0)
	x(i+1) += complex(cos(freq2 * 2 * pi * i / fftLen), sin(freq2 * 2 * pi * i / fftLen));
endif
%for i = 0:samples-1;
%	x(i+1) = complex(cos(freq1 * pi * (i) / fftLen), sin(freq1 * pi * (i) / fftLen));
%	x(i+1) += complex(cos(freq2 * pi * i / fftLen), sin(freq2 * pi * i / fftLen));
%endfor
%if freq2 >= 0
%	x(i+1) .+= complex(cos(freq2 * pi * i / fftLen), sin(freq2 * pi * i * fftLen));
%endif

% add noise
%noise(1:n) = complex( 4 * randn(size(x)), 4 * randn(size(x)));
%x .+= noise;

% read the DFB filter coefficients, then normalize them so the sum = 1
h = dlmread(filterFile);
h = reshape(h, [1 L]);
%h = ones(1, L);
%h(1:fftLen) = zeros(1, fftLen);
h /= sum(h);

% read the post-synthesis filter
h1 = dlmread("LS8c1f");
h1 /= sum(h1);

% initialize the channel output array; the array is organized as
% row = spectrum, column = channel.  DC is the middle channel.
channel = zeros(iter, fftLen);

% loop to perform all dfb's, creating a 2-D matrix.
for i = 0:iter-1
 	% get next block of time samples
	% printf("index = %d\n", i*R);
	y = x(i*R+1:i*R+L);
	% reset the window sum
	s = zeros(1, fftLen);
	% create the sum
	for j = 1:fftLen:L-1
		s += y(j:j+fftLen-1) .* h(j:j+fftLen-1);
	endfor
	% rotate the time samples to correct phase for the overlap
	rotate=-mod(i*overlap, fftLen);
%	plot(real(y(1:fftLen)));
%	input("<");
	s = circshift(s, [0, rotate]);
%	plot(real(s));
%	input(">");
	% compute the FFT of the window
	z = fft(s);
	z = fftshift(z);
	%store the output data in frequency channels
	for j = 1:fftLen
		channel(i+1, j) = z(j);
	endfor 
endfor
%plot(abs(real(out)));
%title("abs(real(out))");
%input("next");
%plot(real(out));
%title("real(out)");

% synthesize a filtered version of original time series from the channels;
% this is done by performing an inverse FFT on each of the rows
% of the channel matrix, which represent spectra.
x1 = zeros(1, fftLen + (iter-1) * R);
for i = 0:iter-1
	% get next spectrum
	y= fftshift(channel(i+1, 1:fftLen));
	z = ifft(y);
	% rotate the data to correct the phase
	rotate = mod(i*overlap, fftLen);
	z = circshift(z, [0, rotate]);
	% apply a post-ifft filter
%	for j = 1:fftLen
%		z(j) *= h1(j);
%	endfor
	% combine with the previous samples
	x1(i*R+1:i*R+fftLen) = z;
endfor
	

