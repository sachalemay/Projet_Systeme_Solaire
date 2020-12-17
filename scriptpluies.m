pluiedata= fopen('pluies.txt','r');
i=1;
while ~feof(pluiedata)
    ligne = fgetl(pluiedata);
    [date,rain] = strtok(ligne, char(9));
    rain = strrep(rain, char(9),'');
    rain = str2double(rain);
    date = datenum(date,'yyyy-mm-dd HH:MM:SS');
    tableau (i,1) = {date};
    tableau (i,2) = {rain};
    i = i+1;
end

ok = 'pasok';
while ~strcmp(ok,'ok')
    labels = {'Date debut', 'Date fin', 'Type de graphique: L=ligne B=barres','Graduation: 1=1jour 6=6heures'};
    li_cols = [1, 15; 1, 15; 1, 40; 1, 30];
    date_debut= datestr(tableau{1,1},'yyyy-mm-dd');
    date_fin = datestr(tableau{end,1},'yyyy-mm-dd');
    val_def = {date_debut, date_fin, 'B', '1'};
    vec_cel = inputdlg(labels, 'Paramètres du graphique', li_cols, val_def);
    if size(vec_cel) == [0 0]
        msg={'Surprise mother fucker!'};
        msgbox(msg, 'Sortir', 'none'); 
        ok = 'ok';
    elseif ~or(strcmp(vec_cel(3,1), 'B'),strcmp(vec_cel{3,1},'L'))
        msg={['Type de graphique ''' vec_cel{3,1} ''' erronné!']};
        uiwait(errordlg(msg, 'Erreur de saisie')); 
    elseif ~or(strcmp(vec_cel{4,1}, '1'),strcmp( vec_cel{4,1}, '6'))
        msg={['Type de graduation ''' vec_cel{4,1} ''' erronné!']};
        uiwait(errordlg(msg, 'Erreur de saisie')); 
    else
        vect_time = [tableau{:,1}];
        vect_rain = [tableau{:,2}];
        plot(vect_time,vect_rain)
        ok = 'ok';
    end
end
