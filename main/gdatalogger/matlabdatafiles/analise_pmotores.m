clear all
close all
load('data.mat');
size = (pitch_angle_size-1);
eval(['pitch_angle' '=' 'pitch_angle_0_' num2str(size) ';']);
eval(['roll_angle' '=' 'roll_angle_0_' num2str(size) ';']);

size = (tempo_size - 1);
eval(['tsim' '=' 'tempo_0_' num2str(size) '(' num2str(size) ')' ';']);
eval(['tempo' '=' 'tempo_0_' num2str(size) ';']);

if (tempo_size ~= pitch_angle_size)
    t = (0:length(pitch_angle)-1)*tsim/(length(pitch_angle)-1);
else
    t = tempo;
end

for i=1:12
if i == 1 || i == 4 || i == 7 || i == 10
eval(['size' '=' 'p_motor' num2str(i) '_size -1;']);
eval(['p_motor' '=' 'p_motor' num2str(i) '_0_' num2str(size) ';']);

if (tempo_size ~= p_motor1_size)
    t = (0:length(p_motor)-1)*tsim/(length(p_motor)-1);
else
    t = tempo;
end

a=plot(t,p_motor);
set(a,'Color','yellow');
hold on
end
end

if (tempo_size ~= roll_angle_size)
    t = (0:length(roll_angle)-1)*tsim/(length(roll_angle)-1);
else
    t = tempo;
end


b=plot(t,-roll_angle);
set(b,'Color','red');
title('Posicao dos Motores Roll x Tempo');
xlabel('Tempo (s)');
ylabel('Angulo (grau)');