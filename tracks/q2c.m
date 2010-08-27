clear;

%cd 'F:\fried\physik\quantumminigolf\octave\quantum2classic'

[pict] = imread('tunnel.bmp');

core = pict(:, :, 1) == 255;

kr=2.5;
[x, y] = meshgrid(1:640, 1:320);
kernel = sqrt((x-320).*(x-320) + (y-160).*(y-160))<kr*kr;
kernel = fftshift(kernel);

conv = abs(ifft2(fft2(core).*fft2(kernel)));
%colormap(gray)
conv = 255 *(conv > 1);
hard(:, :, 1) = conv;
hard(:, :, 2) = conv;
hard(:, :, 3) = conv;
imwrite(hard, 'tunnel_hard.bmp');

pict(find(pict == 255)) = 0;
imwrite(pict, 'tunnel_soft.bmp');