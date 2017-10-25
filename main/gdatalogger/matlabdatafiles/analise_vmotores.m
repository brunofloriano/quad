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

plot(t,pitch_angle);
hold on
plot(t,roll_angle);
legend('Arfagem','Rolagem');
title('Angulos x Tempo');
xlabel('Tempo (s)');
ylabel('Angulo (^o)');

figure



for i=1:12
if i == 1 || i == 4 || i == 7 || i == 10
eval(['size' '=' 'p_motor' num2str(i) '_size -1;']);
eval(['p_motor' '=' 'p_motor' num2str(i) '_0_' num2str(size) ';']);

if (tempo_size ~= v_motor1_size)
    t = (0:length(v_motor)-1)*tsim/(length(p_motor)-1);
else
    t = tempo;
end

a=plot(t,p_motor);
set(a,'Color','yellow');
hold on
end
end

if (tempo_size ~= roll_speed_size)
    t = (0:length(roll_speed)-1)*tsim/(length(roll_speed)-1);
else
    t = tempo;
end


b=plot(t,roll_speed);
set(b,'Color','red');
title('Velocidade dos Motores Roll x Tempo');
xlabel('Tempo (s)');
ylabel('Velocidade (rad/s)');

figure

for i=1:12
if i == 2 || i == 3 || i == 5 || i == 6 || i == 8 || i == 9 || i == 11 || i == 12
size = (v_motor1_size-1);
eval(['size' '=' 'num2str(v_motor' num2str(i) '_size -1);']);
eval(['v_motor' '=' 'v_motor' num2str(i) '_0_' num2str(size) ';']);

if (tempo_size ~= v_motor2_size)
    t = (0:length(v_motor)-1)*tsim/(length(v_motor)-1);
else
    t = tempo;
end

a=plot(t,v_motor);
set(a,'Color','yellow');
hold on
end
end

if (tempo_size ~= pitch_speed_size)
    t = (0:length(pitch_speed)-1)*tsim/(length(pitch_speed)-1);
else
    t = tempo;
end

b=plot(t,pitch_speed);
set(b,'Color','red');
title('Velocidade dos Motores Pitch x Tempo');
xlabel('Tempo (s)');
ylabel('Velocidade (rad/s)');

size = num2str(T_size-1);
eval(['T' '=' 'T_0_' num2str(size) ';']);
media_tempo = mean(T)