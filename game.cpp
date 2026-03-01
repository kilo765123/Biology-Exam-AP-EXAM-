#include <emscripten.h>
#include <string>
#include <vector>
#include <random>
#include <algorithm>

#define EXPORT EMSCRIPTEN_KEEPALIVE

struct Question { std::string q, o[4], a, h; bool m[4]; };
struct Boss { std::string name; int maxHP; std::vector<Question> pool; };
// FIX: Mengembalikan location, atkLevel, dan level ke dalam Player
struct Player { int level=1, currentHP=1000, maxHP=1000, baseAttack=100, atkLevel=1, sp=10, money=500, luckLevel=1, location=0; };

Player p;
Boss exam;
Question curQ;
int m_currentHP;
std::string state = "menu"; 
int last_dmg_to_boss=0, last_dmg_to_player=0, final_score=0, q_answered=0;
bool last_hit_crit=false;
std::mt19937 rng;

void load_new_question() {
    if (exam.pool.empty()) return;   
    curQ = exam.pool[rng() % exam.pool.size()];
    std::shuffle(curQ.o, curQ.o + 4, rng);
    for(int i=0; i<4; i++) curQ.m[i] = false;
}

void init_data() {
    exam.pool.clear();
    exam.name = "UJIAN FINAL BIOLOGI (200 SOAL)";
    exam.maxHP = 20000; // 200 soal x 100 dmg

    // BAB 1: PERTUMBUHAN (25 Soal)
    exam.pool.push_back({"Pertumbuhan bersifat tidak dapat balik disebut?", {"Irreversible", "Reversible", "Kualitatif", "Kuantitatif"}, "Irreversible", "Tak kembali", {0,0,0,0}});
    exam.pool.push_back({"Perkembangan makhluk hidup menuju kedewasaan bersifat?", {"Kualitatif", "Kuantitatif", "Irreversible", "Terukur"}, "Kualitatif", "Tak diukur angka", {0,0,0,0}});
    exam.pool.push_back({"Keluarnya radikula menembus kulit biji disebut?", {"Perkecambahan", "Dormansi", "Fertilisasi", "Absisi"}, "Perkecambahan", "Awal biji hidup", {0,0,0,0}});
    exam.pool.push_back({"Air masuk ke biji saat awal perkecambahan melalui?", {"Imbibisi", "Osmosis", "Difusi", "Transpor"}, "Imbibisi", "Penyusupan air", {0,0,0,0}});
    exam.pool.push_back({"Perkecambahan kotiledon terangkat ke atas tanah dinamakan?", {"Epigeal", "Hipogeal", "Koleoptil", "Skutelum"}, "Epigeal", "Epi=atas", {0,0,0,0}});
    exam.pool.push_back({"Perkecambahan kotiledon tertinggal di bawah tanah disebut?", {"Hipogeal", "Epigeal", "Epikotil", "Plumula"}, "Hipogeal", "Hipo=bawah", {0,0,0,0}});
    exam.pool.push_back({"Calon akar pada embrio biji dinamakan?", {"Radikula", "Plumula", "Koleoptil", "Epikotil"}, "Radikula", "Akar primer", {0,0,0,0}});
    exam.pool.push_back({"Calon daun dan batang embrio biji dinamakan?", {"Plumula", "Radikula", "Koleoriza", "Hipokotil"}, "Plumula", "Pucuk daun", {0,0,0,0}});
    exam.pool.push_back({"Kemampuan menunda pertumbuhan biji saat lingkungan buruk?", {"Dormansi", "Etiolasi", "Vernalisasi", "Absisi"}, "Dormansi", "Tidur panjang", {0,0,0,0}});
    exam.pool.push_back({"Pertumbuhan memanjang ujung akar/batang disebut?", {"Primer", "Sekunder", "Tersier", "Lateral"}, "Primer", "Meristem apikal", {0,0,0,0}});
    exam.pool.push_back({"Jaringan yang menyebabkan batang tumbuhan dikotil membesar?", {"Kambium", "Xilem", "Floem", "Epidermis"}, "Kambium", "Meristem lateral", {0,0,0,0}});
    exam.pool.push_back({"Tumbuh cepat tapi pucat dan lemah di tempat gelap disebut?", {"Etiolasi", "Fototropisme", "Geotropisme", "Nasti"}, "Etiolasi", "Kurang klorofil", {0,0,0,0}});
    exam.pool.push_back({"Hormon pemanjangan sel dan dominansi apikal?", {"Auksin", "Giberelin", "Sitokinin", "Etilen"}, "Auksin", "Diproduksi di pucuk", {0,0,0,0}});
    exam.pool.push_back({"Hormon yang mudah rusak jika terkena cahaya matahari?", {"Auksin", "Sitokinin", "Asam Absisat", "Giberelin"}, "Auksin", "Menyebabkan fototropisme", {0,0,0,0}});
    exam.pool.push_back({"Ujung batang membelok ke arah datangnya cahaya disebut?", {"Fototropisme", "Geotropisme", "Fotonasti", "Taksis"}, "Fototropisme", "Efek auksin", {0,0,0,0}});
    exam.pool.push_back({"Hormon pemecah dormansi biji dan pemicu buah tanpa biji?", {"Giberelin", "Auksin", "Sitokinin", "Kalin"}, "Giberelin", "Partenokarpi", {0,0,0,0}});
    exam.pool.push_back({"Hormon pemicu pembelahan sel & menunda penuaan daun?", {"Sitokinin", "Asam Absisat", "Etilen", "Traumalin"}, "Sitokinin", "Sitokinesis", {0,0,0,0}});
    exam.pool.push_back({"Hormon berbentuk gas yang merangsang pematangan buah?", {"Etilen", "Auksin", "Rizokalin", "Giberelin"}, "Etilen", "Pemeraman", {0,0,0,0}});
    exam.pool.push_back({"Hormon stres pemicu gugur daun & tutup stomata kekeringan?", {"Asam Absisat", "Asam Traumalin", "Sitokinin", "Kalin"}, "Asam Absisat", "Inhibitor", {0,0,0,0}});
    exam.pool.push_back({"Hormon penyembuh luka pada tumbuhan dikotil?", {"Asam Traumalin", "Kalin", "Etilen", "Auksin"}, "Asam Traumalin", "Trauma=luka", {0,0,0,0}});
    exam.pool.push_back({"Hormon pembentuk organ akar disebut?", {"Rizokalin", "Kaulokalin", "Filokalin", "Antokalin"}, "Rizokalin", "Rizo=akar", {0,0,0,0}});
    exam.pool.push_back({"Unsur pusat pengikat molekul klorofil penyebab daun hijau?", {"Magnesium (Mg)", "Besi (Fe)", "Kalsium", "Seng"}, "Magnesium (Mg)", "Mineral makro", {0,0,0,0}});
    exam.pool.push_back({"Daun menguning akibat kekurangan unsur hara disebut?", {"Klorosis", "Nekrosis", "Etiolasi", "Absisi"}, "Klorosis", "Kurang N atau Mg", {0,0,0,0}});
    exam.pool.push_back({"Fase embrionik hewan berbentuk bola sel pejal disebut?", {"Morula", "Blastula", "Gastrula", "Organogenesis"}, "Morula", "Mirip buah arbei", {0,0,0,0}});
    exam.pool.push_back({"Hormon pemicu pergantian kulit (ekdisis) pada serangga?", {"Ekdison", "Juvenil", "Tiroksin", "Adrenalin"}, "Ekdison", "Melepas rangka luar", {0,0,0,0}});

    // BAB 2: METABOLISME (25 Soal)
    exam.pool.push_back({"Reaksi perombakan molekul kompleks menjadi sederhana pembentuk energi?", {"Katabolisme", "Anabolisme", "Kemosintesis", "Asimilasi"}, "Katabolisme", "Eksergonik", {0,0,0,0}});
    exam.pool.push_back({"Reaksi penyusunan senyawa dari molekul sederhana butuh energi?", {"Anabolisme", "Katabolisme", "Respirasi", "Fermentasi"}, "Anabolisme", "Endergonik", {0,0,0,0}});
    exam.pool.push_back({"Senyawa protein fungsional biokatalisator dalam sel?", {"Enzim", "Hormon", "Lipid", "Vitamin"}, "Enzim", "Percepat reaksi", {0,0,0,0}});
    exam.pool.push_back({"Bagian enzim yang tersusun atas protein murni disebut?", {"Apoenzim", "Koenzim", "Kofaktor", "Holoenzim"}, "Apoenzim", "Labil terhadap panas", {0,0,0,0}});
    exam.pool.push_back({"Celah tempat mengikat spesifik molekul substrat pada enzim?", {"Sisi Aktif", "Sisi Alosterik", "Sisi Pasif", "Regulasi"}, "Sisi Aktif", "Kunci dan gembok", {0,0,0,0}});
    exam.pool.push_back({"Teori sisi aktif enzim kaku dan pas dengan substrat?", {"Lock and Key", "Induced Fit", "Allosteric", "Transition"}, "Lock and Key", "Teori gembok kunci", {0,0,0,0}});
    exam.pool.push_back({"Zat penghambat yang bersaing menduduki sisi aktif enzim?", {"Inhibitor Kompetitif", "Non-kompetitif", "Aktivator", "Koenzim"}, "Inhibitor Kompetitif", "Bentuk mirip substrat", {0,0,0,0}});
    exam.pool.push_back({"Kerusakan permanen struktur enzim akibat suhu panas ekstrem?", {"Denaturasi", "Koagulasi", "Aktivasi", "Katalisasi"}, "Denaturasi", "Diatas 60 derajat C", {0,0,0,0}});
    exam.pool.push_back({"Tahap pemecahan 1 Glukosa menjadi 2 Asam Piruvat?", {"Glikolisis", "Siklus Krebs", "Transpor Elektron", "DO"}, "Glikolisis", "Terjadi di sitoplasma", {0,0,0,0}});
    exam.pool.push_back({"Hasil bersih glikolisis 1 molekul glukosa adalah?", {"2 Piruvat, 2 ATP, 2 NADH", "2 Asetil, 2 CO2", "34 ATP, H2O", "Laktat"}, "2 Piruvat, 2 ATP, 2 NADH", "Sitosol", {0,0,0,0}});
    exam.pool.push_back({"Tahap pengubahan Asam Piruvat menjadi Asetil Ko-A?", {"Dekarboksilasi Oksidatif", "Glikolisis", "Krebs", "Transpor"}, "Dekarboksilasi Oksidatif", "Melepas CO2", {0,0,0,0}});
    exam.pool.push_back({"Siklus Krebs (Asam Sitrat) berlangsung di bagian organel?", {"Matriks Mitokondria", "Sitosol", "Krista", "Golgi"}, "Matriks Mitokondria", "Cairan dalam mitokondria", {0,0,0,0}});
    exam.pool.push_back({"Tahap respirasi aerob yang memanen ATP terbanyak?", {"Sistem Transpor Elektron", "Siklus Krebs", "Glikolisis", "DO"}, "Sistem Transpor Elektron", "Di membran krista", {0,0,0,0}});
    exam.pool.push_back({"Penerima elektron terakhir pada ujung rantai transpor aerob?", {"Oksigen (O2)", "CO2", "Hidrogen", "Nitrogen"}, "Oksigen (O2)", "Gabung dengan H+ jadi air", {0,0,0,0}});
    exam.pool.push_back({"Total energi ATP maksimal respirasi aerob dari 1 glukosa?", {"38 ATP", "2 ATP", "34 ATP", "36 ATP"}, "38 ATP", "Sangat efisien", {0,0,0,0}});
    exam.pool.push_back({"Otot yang kelelahan dan kurang oksigen melakukan respirasi?", {"Fermentasi Asam Laktat", "Alkohol", "Krebs", "Kemosintesis"}, "Fermentasi Asam Laktat", "Menyebabkan pegal linu", {0,0,0,0}});
    exam.pool.push_back({"Total ATP bersih yang dihasilkan fermentasi anaerob 1 glukosa?", {"2 ATP", "36 ATP", "38 ATP", "4 ATP"}, "2 ATP", "Sangat sedikit", {0,0,0,0}});
    exam.pool.push_back({"Tumpukan membran tilakoid kloroplas tempat Reaksi Terang?", {"Grana", "Stroma", "Matriks", "Krista"}, "Grana", "Berisi fotosistem hijau", {0,0,0,0}});
    exam.pool.push_back({"Cairan di dalam kloroplas tempat terjadinya Reaksi Gelap?", {"Stroma", "Grana", "Matriks", "Sitosol"}, "Stroma", "Ada enzim rubisco", {0,0,0,0}});
    exam.pool.push_back({"Proses pemecahan molekul air (H2O) oleh energi cahaya matahari?", {"Fotolisis", "Fosforilasi", "Karboksilasi", "Reduksi"}, "Fotolisis", "Menghasilkan gas oksigen", {0,0,0,0}});
    exam.pool.push_back({"Hasil Reaksi Terang yang digunakan sebagai energi Reaksi Gelap?", {"ATP dan NADPH", "O2 dan Glukosa", "Oksigen", "Air"}, "ATP dan NADPH", "Dikirim ke stroma", {0,0,0,0}});
    exam.pool.push_back({"Enzim utama pengikat gas CO2 pada tumbuhan C3?", {"Rubisco", "PEP Karboksilase", "Amilase", "Katalase"}, "Rubisco", "Enzim terpekat di bumi", {0,0,0,0}});
    exam.pool.push_back({"Tumbuhan C4 (jagung, tebu) memfiksasi CO2 menggunakan enzim?", {"PEP Karboksilase", "Rubisco", "Amilase", "Katalase"}, "PEP Karboksilase", "Lebih efisien di suhu panas", {0,0,0,0}});
    exam.pool.push_back({"Sintesis energi oleh bakteri menggunakan reaksi oksidasi kimiawi?", {"Kemosintesis", "Fotosintesis", "Fermentasi", "Katabolisme"}, "Kemosintesis", "Bakteri nitrifikasi", {0,0,0,0}});
    exam.pool.push_back({"Enzim penetral racun hidrogen peroksida menjadi air dan oksigen?", {"Katalase", "Pepsin", "Lipase", "Amilase"}, "Katalase", "Banyak di hati dan jantung", {0,0,0,0}});

    // BAB 3: BAKTERI (25 Soal)
    exam.pool.push_back({"Kingdom Monera (bakteri) beranggotakan sel dengan tipe?", {"Prokariotik", "Eukariotik", "Multiseluler", "Jaringan"}, "Prokariotik", "Tidak ada membran inti", {0,0,0,0}});
    exam.pool.push_back({"Cara reproduksi aseksual utama yang dilakukan bakteri?", {"Pembelahan Biner", "Konjugasi", "Transformasi", "Transduksi"}, "Pembelahan Biner", "Membelah jadi dua", {0,0,0,0}});
    exam.pool.push_back({"DNA sirkuler ekstra bakteri yang sering dipakai rekayasa genetik?", {"Plasmid", "Nukleoid", "Ribosom", "Mesosom"}, "Plasmid", "Bisa berpindah antar sel", {0,0,0,0}});
    exam.pool.push_back({"Dinding sel bakteri Eubacteria sejati utamanya tersusun dari?", {"Peptidoglikan", "Selulosa", "Kitin", "Lignin"}, "Peptidoglikan", "Protein dan gula", {0,0,0,0}});
    exam.pool.push_back({"Transfer materi genetik antar bakteri melalui perantara virus?", {"Transduksi", "Transformasi", "Konjugasi", "Biner"}, "Transduksi", "Virus bakteriofag", {0,0,0,0}});
    exam.pool.push_back({"Pertukaran plasmid dua bakteri dengan jembatan pili seks?", {"Konjugasi", "Transduksi", "Transformasi", "Mutasi"}, "Konjugasi", "Perkawinan bakteri", {0,0,0,0}});
    exam.pool.push_back({"Pengambilan DNA bebas dari sel bakteri yang sudah mati?", {"Transformasi", "Transduksi", "Konjugasi", "Plasmid"}, "Transformasi", "Eksperimen tikus Griffith", {0,0,0,0}});
    exam.pool.push_back({"Bentuk bakteri melengkung seperti tanda koma penyebab kolera?", {"Vibrio", "Basil", "Kokus", "Spirilum"}, "Vibrio", "Vibrio cholerae", {0,0,0,0}});
    exam.pool.push_back({"Bakteri bentuk bulat bergerombol mirip dompolan anggur?", {"Stafilokokus", "Streptokokus", "Diplokokus", "Sarkina"}, "Stafilokokus", "Stafilo = anggur", {0,0,0,0}});
    exam.pool.push_back({"Bakteri bulat yang berbaris memanjang seperti rantai?", {"Streptokokus", "Stafilokokus", "Diplokokus", "Sarkina"}, "Streptokokus", "Strepto = rantai", {0,0,0,0}});
    exam.pool.push_back({"Archaebacteria rawa & usus sapi penghasil gas metana?", {"Metanogen", "Halofil", "Termoasidofil", "Sianobakteri"}, "Metanogen", "CH4", {0,0,0,0}});
    exam.pool.push_back({"Archaebacteria yang hidup di lingkungan salinitas garam ekstrem?", {"Halofil ekstrem", "Metanogen", "Termo", "Sulfur"}, "Halofil ekstrem", "Halo = laut/garam", {0,0,0,0}});
    exam.pool.push_back({"Archaebacteria habitat kawah gunung berapi panas dan asam?", {"Termoasidofil", "Halofil", "Metanogen", "Gram positif"}, "Termoasidofil", "Suhu tinggi pH rendah", {0,0,0,0}});
    exam.pool.push_back({"Fase dorman bakteri berlapis tebal tahan cuaca ekstrem mematikan?", {"Endospora", "Kapsul", "Pili", "Plasmid"}, "Endospora", "Pelindung DNA terdalam", {0,0,0,0}});
    exam.pool.push_back({"Bakteri pengikat Nitrogen di bintil akar kacang polong?", {"Rhizobium", "E. coli", "Salmonella", "Lactobacillus"}, "Rhizobium", "Penyubur tanah alami", {0,0,0,0}});
    exam.pool.push_back({"Alat gerak bakteri seperti bulu cambuk protein panjang?", {"Flagela", "Silia", "Pili", "Kapsul"}, "Flagela", "Memutar seperti baling", {0,0,0,0}});
    exam.pool.push_back({"Tipe bakteri dengan flagela menyebar di seluruh tubuhnya?", {"Peritrik", "Monotrik", "Lofotrik", "Amfitrik"}, "Peritrik", "Peri = sekujur permukaan", {0,0,0,0}});
    exam.pool.push_back({"Bakteri Gram-positif menyerap zat warna Gram menjadi warna?", {"Ungu/Kristal Violet", "Merah muda", "Hijau", "Bening"}, "Ungu/Kristal Violet", "Karena dindingnya tebal", {0,0,0,0}});
    exam.pool.push_back({"Bakteri fermentasi pembuat Nata de Coco dari air kelapa?", {"Acetobacter xylinum", "Lactobacillus", "Rhizobium", "E.coli"}, "Acetobacter xylinum", "Hasilkan selulosa", {0,0,0,0}});
    exam.pool.push_back({"Bakteri penyebab infeksi paru-paru TBC mematikan?", {"Mycobacterium tuberculosis", "Salmonella", "Vibrio", "Neisseria"}, "Mycobacterium tuberculosis", "MTB", {0,0,0,0}});
    exam.pool.push_back({"Bakteri penyebab penyakit kelamin Sifilis (raja singa) spiral?", {"Treponema pallidum", "Neisseria", "Chlamydia", "HIV"}, "Treponema pallidum", "Bentuk spiroseta", {0,0,0,0}});
    exam.pool.push_back({"Bakteri fotosintetik (Cyanobacteria) punya pigmen biru khas?", {"Fikosianin", "Fikoeritrin", "Klorofil", "Xantofil"}, "Fikosianin", "Alga hijau-biru", {0,0,0,0}});
    exam.pool.push_back({"Sel Cyanobacteria berdinding tebal khusus fiksasi Nitrogen?", {"Heterokista", "Akinet", "Baeosit", "Hormogonium"}, "Heterokista", "Spesialis pengikat udara", {0,0,0,0}});
    exam.pool.push_back({"Bakteri E. coli di usus besar manusia bersimbiosis membentuk?", {"Vitamin K", "Diare", "Glikogen", "Selulosa"}, "Vitamin K", "Pembusukan sisa makanan", {0,0,0,0}});
    exam.pool.push_back({"Lendir luar bakteri patogen pelindung dari sel darah putih?", {"Kapsul", "Pili", "Flagel", "Membran plasma"}, "Kapsul", "Anti fagositosis", {0,0,0,0}});

    // BAB 4: SISTEM GERAK (25 Soal)
    exam.pool.push_back({"Tulang pada sistem manusia bertindak sebagai alat gerak?", {"Pasif", "Aktif", "Sinergis", "Antagonis"}, "Pasif", "Otot penggerak utamanya", {0,0,0,0}});
    exam.pool.push_back({"Kelainan tulang bengkok X/O akibat kurang Vitamin D balita?", {"Rakhitis", "Polio", "Osteoporosis", "Tetanus"}, "Rakhitis", "Sinar matahari pagi", {0,0,0,0}});
    exam.pool.push_back({"Hubungan antar tulang tengkorak yang tidak bisa gerak mati?", {"Sinartrosis", "Diartrosis", "Amfiartrosis", "Sinergis"}, "Sinartrosis", "Sutura rapat", {0,0,0,0}});
    exam.pool.push_back({"Contoh tulang berbentuk pipih di rangka manusia?", {"Tulang rusuk", "Tulang paha", "Tulang betis", "Tulang jari"}, "Tulang rusuk", "Pelindung jantung", {0,0,0,0}});
    exam.pool.push_back({"Dua otot bekerja berlawanan arah (Biceps dan Triceps)?", {"Antagonis", "Sinergis", "Ekstensor", "Depresor"}, "Antagonis", "Satu tarik satu relaksasi", {0,0,0,0}});
    exam.pool.push_back({"Tulang keras menyimpan endapan mineral utama berupa?", {"Kalsium dan Fosfor", "Besi", "Kalium", "Natrium"}, "Kalsium dan Fosfor", "Pembentuk matriks kaku", {0,0,0,0}});
    exam.pool.push_back({"Tulang punggung bungkuk melengkung ke belakang dada?", {"Kifosis", "Lordosis", "Skoliosis", "Rakhitis"}, "Kifosis", "Bungkuk nenek", {0,0,0,0}});
    exam.pool.push_back({"Tulang punggung membengkok ke samping mirip huruf S?", {"Skoliosis", "Lordosis", "Kifosis", "Polio"}, "Skoliosis", "Duduk miring", {0,0,0,0}});
    exam.pool.push_back({"Tulang punggung bengkok perut membusung depan?", {"Lordosis", "Kifosis", "Skoliosis", "Rakhitis"}, "Lordosis", "Ibu hamil", {0,0,0,0}});
    exam.pool.push_back({"Kaku otot rahang infeksi bakteri luka paku berkarat?", {"Tetanus", "Atrofi", "Hipertrofi", "Kram"}, "Tetanus", "Clostridium tetani", {0,0,0,0}});
    exam.pool.push_back({"Sendi pangkal lengan bahu bisa bergerak bebas mutar?", {"Sendi Peluru", "Sendi Engsel", "Putar", "Pelana"}, "Sendi Peluru", "Bonggol masuk mangkok", {0,0,0,0}});
    exam.pool.push_back({"Sendi siku dan lutut hanya gerak ke 1 arah menekuk?", {"Sendi Engsel", "Sendi Peluru", "Pelana", "Putar"}, "Sendi Engsel", "Seperti engsel pintu", {0,0,0,0}});
    exam.pool.push_back({"Proses pengerasan tulang rawan (kartilago) menjadi tulang?", {"Osifikasi", "Kondritis", "Osteoporosis", "Artikulasi"}, "Osifikasi", "Penulangan janin", {0,0,0,0}});
    exam.pool.push_back({"Sel pemakan/perombak matriks tulang tua bentuk rongga?", {"Osteoklas", "Osteoblas", "Osteosit", "Kondrosit"}, "Osteoklas", "Klas=hancur", {0,0,0,0}});
    exam.pool.push_back({"Gerak membengkokkan/menekuk lengan disebut gerak otot?", {"Fleksi", "Ekstensi", "Abduksi", "Adduksi"}, "Fleksi", "Lawan lurus", {0,0,0,0}});
    exam.pool.push_back({"Gerak merentangkan tangan menjauhi sumbu tubuh (lebar)?", {"Abduksi", "Adduksi", "Elevasi", "Depresi"}, "Abduksi", "Jauh dari badan", {0,0,0,0}});
    exam.pool.push_back({"Jaringan ikat penghubung antartulang pada sendi agar tak lepas?", {"Ligamen", "Tendon", "Fasia", "Kapsul"}, "Ligamen", "Karet sendi", {0,0,0,0}});
    exam.pool.push_back({"Urat kuat penghubung otot rangka dengan tulang (Achilles)?", {"Tendon", "Ligamen", "Rongga", "Kondrin"}, "Tendon", "Penarik tulang", {0,0,0,0}});
    exam.pool.push_back({"Oli/pelumas alami rongga sendi pencegah gesekan tulang?", {"Cairan Sinovial", "Limfa", "Darah", "Plasma"}, "Cairan Sinovial", "Kapsul pelumas", {0,0,0,0}});
    exam.pool.push_back({"Otot rangka bekerja secara sadar, inti sel banyak di tepi?", {"Otot Lurik", "Otot Polos", "Jantung", "Sfingter"}, "Otot Lurik", "Melekat di tulang", {0,0,0,0}});
    exam.pool.push_back({"Otot lambung pencernaan tak sadar bentuk gelendong?", {"Otot Polos", "Otot Lurik", "Jantung", "Rangka"}, "Otot Polos", "Tidak mudah lelah", {0,0,0,0}});
    exam.pool.push_back({"Otot menyusut mengecil lumpuh tak digerakkan berbulan-bulan?", {"Atrofi", "Hipertrofi", "Distrofi", "Kram"}, "Atrofi", "Efek stroke/gips", {0,0,0,0}});
    exam.pool.push_back({"Unit struktur pita kontraksi aktin dan miosin otot lurik?", {"Sarkomer", "Sarkoplasma", "Osteon", "Kondrosit"}, "Sarkomer", "Garis Z ke Z", {0,0,0,0}});
    exam.pool.push_back({"Tulang rapuh keropos penyakit lansia hilangnya kalsium?", {"Osteoporosis", "Rakhitis", "Polio", "Fraktura"}, "Osteoporosis", "Tulang berpori", {0,0,0,0}});
    exam.pool.push_back({"Istilah medis patah tulang terbelah dua cedera kecelakaan?", {"Fraktura", "Fisura", "Osteoporosis", "Osteoartritis"}, "Fraktura", "Tindakan gips", {0,0,0,0}});

    // BAB 5: SISTEM EKSKRESI (25 Soal)
    exam.pool.push_back({"Organ utama pembentuk urine yang menyaring plasma darah?", {"Ginjal", "Hati", "Paru", "Kulit"}, "Ginjal", "Ren", {0,0,0,0}});
    exam.pool.push_back({"Unit struktural terkecil saringan dalam ginjal jutaan jumlahnya?", {"Nefron", "Glomerulus", "Ureter", "Uretra"}, "Nefron", "Pabrik urine mikroskopis", {0,0,0,0}});
    exam.pool.push_back({"Tahap filtrasi darah pertama terjadi di gumpalan kapiler?", {"Glomerulus", "Tubulus Proksimal", "Lengkung Henle", "Distal"}, "Glomerulus", "Mangkuk kapsula Bowman", {0,0,0,0}});
    exam.pool.push_back({"Urine hasil saringan awal masih mengandung glukosa disebut?", {"Urine Primer", "Urine Sekunder", "Urine Sejati", "Filtrat"}, "Urine Primer", "Belum diserap", {0,0,0,0}});
    exam.pool.push_back({"Reabsorpsi penyerapan kembali zat bergizi (asam amino) di?", {"Tubulus Proksimal", "Glomerulus", "Distal", "Henle"}, "Tubulus Proksimal", "Awal saluran", {0,0,0,0}});
    exam.pool.push_back({"Augmentasi buang racun sisa obat di tubulus akhir membentuk?", {"Urine Sejati", "Urine Primer", "Darah", "Glukosa"}, "Urine Sejati", "Siap dibuang", {0,0,0,0}});
    exam.pool.push_back({"Saluran pipa mengantar urine dari ginjal ke kandung kemih?", {"Ureter", "Uretra", "Pelvis", "Vesika"}, "Ureter", "Pipa panjang", {0,0,0,0}});
    exam.pool.push_back({"Saluran pembuang urine dari kandung kemih ke luar alat vital?", {"Uretra", "Ureter", "Vesika", "Pelvis"}, "Uretra", "Lubang pipis", {0,0,0,0}});
    exam.pool.push_back({"Hormon pituitari atur serap air ginjal cegah kencing terus?", {"ADH", "Insulin", "Glukagon", "Tiroksin"}, "ADH", "Antidiuretic Hormone", {0,0,0,0}});
    exam.pool.push_back({"Kencing manis, glukosa bocor ke urine akibat kurang insulin?", {"Diabetes Melitus", "Insipidus", "Albuminuria", "Nefritis"}, "Diabetes Melitus", "Gula darah numpuk", {0,0,0,0}});
    exam.pool.push_back({"Radang nefron infeksi bakteri Streptococcus gagal ginjal?", {"Nefritis", "Batu Ginjal", "Albuminuria", "Uremia"}, "Nefritis", "Rusak glomerulus", {0,0,0,0}});
    exam.pool.push_back({"Bocornya protein darah ke urine tanda saringan ginjal rusak?", {"Albuminuria", "Nefritis", "Batu Ginjal", "Hematuria"}, "Albuminuria", "Busa di urin", {0,0,0,0}});
    exam.pool.push_back({"Zat warna kuning pewarna feses urin rombakan eritrosit hati?", {"Bilirubin/Urobilin", "Melanin", "Hemoglobin", "Klorofil"}, "Bilirubin/Urobilin", "Getah empedu", {0,0,0,0}});
    exam.pool.push_back({"Hati menetralkan amonia racun nitrogen diubah jadi cairan?", {"Urea", "Laktat", "Glukosa", "Asam Amino"}, "Urea", "Siklus ornitin", {0,0,0,0}});
    exam.pool.push_back({"Lapisan luar kulit yang selnya mati terus mengelupas tanduk?", {"Epidermis", "Dermis", "Hipodermis", "Subkutan"}, "Epidermis", "Stratum korneum", {0,0,0,0}});
    exam.pool.push_back({"Kelenjar keringat pengatur suhu tubuh terletak di lapisan?", {"Dermis", "Epidermis", "Tulang", "Otot"}, "Dermis", "Glandula sudorifera", {0,0,0,0}});
    exam.pool.push_back({"Kelenjar minyak rambut pelembab pencegah kulit kering retak?", {"Glandula Sebasea", "Sudorifera", "Tiroid", "Adrenal"}, "Glandula Sebasea", "Jerawat bakteri", {0,0,0,0}});
    exam.pool.push_back({"Ekskresi limbah padat asam urat serangga belalang diserap?", {"Pembuluh Malpighi", "Nefridia", "Sel Api", "Paru buku"}, "Pembuluh Malpighi", "Nempel usus", {0,0,0,0}});
    exam.pool.push_back({"Sel api penyaring getar ekskresi pada cacing pipih planaria?", {"Flame Cell", "Nefridia", "Malpighi", "Kloaka"}, "Flame Cell", "Silia menyala", {0,0,0,0}});
    exam.pool.push_back({"Saluran ekskresi tiap segmen ruas pada cacing tanah annelida?", {"Nefridia", "Sel Api", "Malpighi", "Ginjal"}, "Nefridia", "Berlubang-lubang", {0,0,0,0}});
    exam.pool.push_back({"Hati rusak mengeras jaringan parut matinya sel akibat alkohol?", {"Sirosis", "Hepatitis", "Jaundice", "Nefritis"}, "Sirosis", "Sering mematikan", {0,0,0,0}});
    exam.pool.push_back({"Endapan keras kalsium oksalat rongga ginjal sulit kencing?", {"Batu Ginjal", "Tumor", "Kista", "Nefritis"}, "Batu Ginjal", "Kurang minum air putih", {0,0,0,0}});
    exam.pool.push_back({"Mesin ginjal pencuci darah buatan pasien gagal ginjal kronis?", {"Hemodialisis", "Transfusi", "EKG", "Suntik"}, "Hemodialisis", "Filter luar tubuh", {0,0,0,0}});
    exam.pool.push_back({"Pigmen hitam pelindung sel kulit dari kanker radiasi sinar UV?", {"Melanin", "Karoten", "Hb", "Urobilin"}, "Melanin", "Dibuat melanosit", {0,0,0,0}});
    exam.pool.push_back({"Batu empedu menyumbat saluran getah bikin kulit/mata kuning?", {"Jaundice", "Hepatitis", "Sirosis", "Batu Ginjal"}, "Jaundice", "Penyakit kuning", {0,0,0,0}});

    // BAB 6: SISTEM REPRODUKSI (25 Soal)
    exam.pool.push_back({"Tempat utama terjadinya fertilisasi sperma dan ovum wanita?", {"Tuba Fallopi (Oviduk)", "Ovarium", "Uterus", "Vagina"}, "Tuba Fallopi (Oviduk)", "Saluran sel telur", {0,0,0,0}});
    exam.pool.push_back({"Hormon puncak di otak pemicu pelepasan telur dari ovarium?", {"LH (Luteinizing)", "FSH", "Estrogen", "Progesteron"}, "LH (Luteinizing)", "Puncak ovulasi", {0,0,0,0}});
    exam.pool.push_back({"Proses seluler pembentukan 4 sperma di testis jantan disebut?", {"Spermatogenesis", "Oogenesis", "Gametogenesis", "Ovulasi"}, "Spermatogenesis", "Mitosis dan meiosis", {0,0,0,0}});
    exam.pool.push_back({"Saluran panjang berkelok di atas testis tempat sperma matang?", {"Epididimis", "Vas Deferens", "Tubulus", "Uretra"}, "Epididimis", "Penyimpanan sementara", {0,0,0,0}});
    exam.pool.push_back({"Lapisan dalam rahim kaya darah yang luruh tiap bulan haid?", {"Endometrium", "Miometrium", "Perimetrium", "Serviks"}, "Endometrium", "Lapis plasenta embrio", {0,0,0,0}});
    exam.pool.push_back({"Pembelahan Oogenesis 1 sel induk mematikan 3 sel sisanya?", {"1 Ovum fungsional", "4 Ovum", "2 Ovum 2 Polar", "Sperma"}, "1 Ovum fungsional", "Sisanya badan polar", {0,0,0,0}});
    exam.pool.push_back({"Sisa folikel kuning rahim penghasil hormon kehamilan tebal?", {"Korpus Luteum", "Primer", "Sekunder", "Albikans"}, "Korpus Luteum", "Progesteron pabrik", {0,0,0,0}});
    exam.pool.push_back({"Kantong kulit pengatur suhu testis agar produksi sperma ideal?", {"Skrotum", "Epididimis", "Prostat", "Penis"}, "Skrotum", "Di luar tubuh", {0,0,0,0}});
    exam.pool.push_back({"Hormon pria pembentuk suara berat otot dada lebar sel Leydig?", {"Testosteron", "Estrogen", "Progesteron", "Prolaktin"}, "Testosteron", "Ciri kelamin sekunder", {0,0,0,0}});
    exam.pool.push_back({"Topi kepala sperma enzim penghancur pelindung ovum corona?", {"Akrosom", "Lisosom", "Mitokondria", "Sentriol"}, "Akrosom", "Hialuronidase isi", {0,0,0,0}});
    exam.pool.push_back({"Organ elastis super kuat tempat janin berkembang 9 bulan?", {"Uterus (Rahim)", "Ovarium", "Tuba", "Serviks"}, "Uterus (Rahim)", "Otot tebal", {0,0,0,0}});
    exam.pool.push_back({"Sel dalam tubulus seminiferus nutrisi perawat bayi sperma?", {"Sel Sertoli", "Sel Leydig", "Gonium", "Spermatid"}, "Sel Sertoli", "Pemberi makan", {0,0,0,0}});
    exam.pool.push_back({"Hormon ciri wanita dada membesar pinggul lebar dari folikel?", {"Estrogen", "Progesteron", "Testosteron", "Oksitosin"}, "Estrogen", "Hormon feminin utama", {0,0,0,0}});
    exam.pool.push_back({"Hormon pertahan dinding rahim agar tak gugur saat hamil?", {"Progesteron", "Estrogen", "FSH", "LH"}, "Progesteron", "Hormon kehamilan", {0,0,0,0}});
    exam.pool.push_back({"Proses tertanamnya blastokista embrio muda di dinding rahim?", {"Implantasi (Nidasi)", "Fertilisasi", "Kopulasi", "Ovulasi"}, "Implantasi (Nidasi)", "Penempelan sukses", {0,0,0,0}});
    exam.pool.push_back({"Cakram pembuluh darah jembatan O2 nutrisi janin ibu tali pusar?", {"Plasenta (Ari-ari)", "Amnion", "Korion", "Alantois"}, "Plasenta (Ari-ari)", "Keluar pasca persalinan", {0,0,0,0}});
    exam.pool.push_back({"Air ketuban balon peredam benturan janin membran dalam?", {"Amnion", "Plasenta", "Korion", "Alantois"}, "Amnion", "Pecah tanda lahir", {0,0,0,0}});
    exam.pool.push_back({"Hormon pemeras otot rahim kontraksi hebat dorong bayi lahir?", {"Oksitosin", "Prolaktin", "Estrogen", "Relaksin"}, "Oksitosin", "Juga hormon pelukan ibu", {0,0,0,0}});
    exam.pool.push_back({"Hormon kelenjar payudara memproduksi susu pasca persalinan?", {"Prolaktin", "Oksitosin", "LH", "FSH"}, "Prolaktin", "Laktasi = susu", {0,0,0,0}});
    exam.pool.push_back({"Penyakit menular raja singa spiral Treponema pallidum kelamin?", {"Sifilis", "Gonore", "Kencing Nanah", "Herpes"}, "Sifilis", "Menyerang saraf fatal", {0,0,0,0}});
    exam.pool.push_back({"Infeksi Kencing Nanah bakteri mematikan Neisseria gonorrhoeae?", {"Gonore", "Sifilis", "Klamidia", "AIDS"}, "Gonore", "GO singkatannya", {0,0,0,0}});
    exam.pool.push_back({"Virus perusak imun T Helper AIDS nular via darah jarum seks?", {"HIV", "HPV", "HSV", "Hepatitis"}, "HIV", "Tanpa obat sembuh", {0,0,0,0}});
    exam.pool.push_back({"Virus kutil kelamin memicu mematikan Kanker Leher Rahim?", {"HPV", "HIV", "Sifilis", "Herpes"}, "HPV", "Human Papilloma", {0,0,0,0}});
    exam.pool.push_back({"Operasi potong ikat saluran sperma kontrasepsi permanen pria?", {"Vasektomi", "Tubektomi", "Kastrasi", "Sunat"}, "Vasektomi", "Vas Deferens putus", {0,0,0,0}});
    exam.pool.push_back({"Fase tua wanita habisnya sel telur ovarium berhenti menstruasi?", {"Menopause", "Menarche", "Amenorea", "Pubertas"}, "Menopause", "Habis masa subur", {0,0,0,0}});

    // BAB 7: SISTEM IMUN (25 Soal)
    exam.pool.push_back({"Senyawa patogen asing pembawa penyakit memicu respons imun?", {"Antigen", "Antibodi", "Makrofag", "Limfokin"}, "Antigen", "Musuh tubuh masuk", {0,0,0,0}});
    exam.pool.push_back({"Protein bentuk Y spesifik netralisir racun dari sel B plasma?", {"Antibodi", "Antigen", "Histamin", "Interferon"}, "Antibodi", "Imunoglobulin penembak", {0,0,0,0}});
    exam.pool.push_back({"Sel darah putih fagosit raksasa menelan hancurkan bakteri?", {"Makrofag", "Eritrosit", "Trombosit", "Sel T"}, "Makrofag", "Pemakan besar keliling", {0,0,0,0}});
    exam.pool.push_back({"Sel limfosit pembuat antibodi matang di sumsum tulang Bone?", {"Sel B", "Sel T", "Monosit", "Neutrofil"}, "Sel B", "B marrow", {0,0,0,0}});
    exam.pool.push_back({"Sel limfosit pengatur perang imun matang di Kelenjar Timus?", {"Sel T", "Sel B", "Basofil", "Eosinofil"}, "Sel T", "Huruf depannya", {0,0,0,0}});
    exam.pool.push_back({"Protein pertahanan tubuh anti-virus hambat replikasi sel tetangga?", {"Interferon", "Komplemen", "Histamin", "Lisozim"}, "Interferon", "Interferensi virus", {0,0,0,0}});
    exam.pool.push_back({"Mediator pemicu pelebaran pembuluh alergi radang merah bengkak?", {"Histamin", "Interferon", "Lisozim", "Perforin"}, "Histamin", "Dilepas sel mast", {0,0,0,0}});
    exam.pool.push_back({"Enzim pelindung luar hancurkan kuman di air mata dan ludah?", {"Lisozim", "Amilase", "Pepsin", "Lipase"}, "Lisozim", "Barier kimia pertama", {0,0,0,0}});
    exam.pool.push_back({"Imun bayi dari kolostrum air susu ibu gratis tanpa buat sendiri?", {"Pasif Alami", "Aktif Alami", "Pasif Buatan", "Aktif Buatan"}, "Pasif Alami", "Transfer antibodi ibu", {0,0,0,0}});
    exam.pool.push_back({"Suntik kuman lemah (vaksin) paksa tubuh latih sel memori jangka panjang?", {"Aktif Buatan", "Pasif Buatan", "Aktif Alami", "Pasif Alami"}, "Aktif Buatan", "Sengaja disuntik", {0,0,0,0}});
    exam.pool.push_back({"Suntik serum antibodi bisa ular gigitan kobra tindakan darurat?", {"Pasif Buatan", "Aktif Buatan", "Pasif Alami", "Aktif Alami"}, "Pasif Buatan", "Tak ada waktu buat", {0,0,0,0}});
    exam.pool.push_back({"Antibodi darah paling melimpah 80% penjaga janin tembus plasenta?", {"IgG", "IgA", "IgM", "IgE"}, "IgG", "General terbanyak", {0,0,0,0}});
    exam.pool.push_back({"Antibodi pelindung pintu masuk di mukosa usus, keringat, ASI?", {"IgA", "IgG", "IgM", "IgE"}, "IgA", "Lapis pertama cairan", {0,0,0,0}});
    exam.pool.push_back({"Antibodi spesialis pembuat gatal radang alergi & basmi cacing?", {"IgE", "IgG", "IgA", "IgM"}, "IgE", "Nempel di sel mast", {0,0,0,0}});
    exam.pool.push_back({"Sel T pembunuh eksekutor sel infeksi & tumor tembak perforin?", {"Sel T Sitotoksik", "Sel T Helper", "Sel T Supresor", "Sel B"}, "Sel T Sitotoksik", "Killer T", {0,0,0,0}});
    exam.pool.push_back({"Sel T panglima jenderal komando aktivasi sel B (korban virus HIV)?", {"Sel T Helper", "Sitotoksik", "Supresor", "Memori"}, "Sel T Helper", "CD4 penolong", {0,0,0,0}});
    exam.pool.push_back({"Sel B berumur puluhan tahun diam ingat profil musuh vaksin?", {"Sel B Memori", "Sel B Plasma", "Sel T Helper", "Makrofag"}, "Sel B Memori", "Ingatan jangka panjang", {0,0,0,0}});
    exam.pool.push_back({"Sistem imun berlebihan nyerang zat aman serbuk sari gatal asma?", {"Alergi", "Autoimun", "Imunodefisiensi", "Inflamasi"}, "Alergi", "Hipersensitivitas lingkungan", {0,0,0,0}});
    exam.pool.push_back({"Syok gawat darurat alergi obat tensi darah merosot mati bengkak?", {"Anafilaksis", "Asma", "Hepatitis", "Eritema"}, "Anafilaksis", "Butuh epinefrin segera", {0,0,0,0}});
    exam.pool.push_back({"Penyakit salah sasaran imun hancurkan sendi organ tubuh sendiri (Lupus)?", {"Autoimun", "Alergi", "Imunodefisiensi", "HIV"}, "Autoimun", "Makan teman", {0,0,0,0}});
    exam.pool.push_back({"Lumpuh rontoknya sistem imun akibat virus tak ada sel penolong mati AIDS?", {"Imunodefisiensi", "Autoimun", "Alergi", "Vaksin"}, "Imunodefisiensi", "AIDS HIV", {0,0,0,0}});
    exam.pool.push_back({"Cairan getah bening limfatik bawa sel imun kembali ke pembuluh darah?", {"Vena Subklavia", "Aorta", "Ginjal", "Hati"}, "Vena Subklavia", "Di dada atas kiri", {0,0,0,0}});
    exam.pool.push_back({"Cara antibodi ikat lengketkan puluhan kuman jadi gumpalan mudah dimakan?", {"Aglutinasi", "Netralisasi", "Opsonisasi", "Lisis"}, "Aglutinasi", "Saling lekat", {0,0,0,0}});
    exam.pool.push_back({"Senjata protein pelubang membran dinding sel asing hingga bocor hancur?", {"Perforin Komplemen", "Histamin", "Interferon", "Gula"}, "Perforin Komplemen", "Bocor meledak air", {0,0,0,0}});
    exam.pool.push_back({"Bengkak lokal merah panas pembuluh melebar luka pisau duri bakteri?", {"Inflamasi", "Anafilaksis", "Alergi", "Autoimun"}, "Inflamasi", "Reaksi radang lokal luka", {0,0,0,0}});

    // BAB 8: HEREDITAS MANUSIA (25 Soal)
    exam.pool.push_back({"Diagram peta silsilah keluarga pohon lacak penyakit genetik keturunan?", {"Pedigree", "Kariotipe", "Fenotipe", "Kloning"}, "Pedigree", "Diagram pewarisan keluarga", {0,0,0,0}});
    exam.pool.push_back({"Susunan seluruh 46 kromosom genetik sel tubuh wanita sehat normal?", {"22AA + XX", "22AA + XY", "44AA + XX", "23AA + XX"}, "22AA + XX", "Autosom pasangan seks", {0,0,0,0}});
    exam.pool.push_back({"Kromosom seks gonosom penentu jenis kelamin gen pria/ayah?", {"XY", "XX", "XO", "XXY"}, "XY", "Dapat Y dari bapak", {0,0,0,0}});
    exam.pool.push_back({"Penyakit darah pangeran kerajaan Inggris sukar membeku luka kecil mati?", {"Hemofilia", "Thalassemia", "Buta warna", "Albino"}, "Hemofilia", "Terpaut X resesif", {0,0,0,0}});
    exam.pool.push_back({"Wanita sehat tapi punya gen resesif cacat tersembunyi diturunkan ke anak?", {"Carrier", "Letal", "Mutan", "Penderita"}, "Carrier", "Heterozigot bawa", {0,0,0,0}});
    exam.pool.push_back({"Cacat albino genetik resesif kulit bule rambut jagung genotipe aa tanpa?", {"Pigmen Melanin", "Hemoglobin", "Klorofil", "Keratin"}, "Pigmen Melanin", "Penangkal UV", {0,0,0,0}});
    exam.pool.push_back({"Mutasi kelainan dominan jari tangan & kaki bertambah lebih dari lima?", {"Polidaktili", "Brakidaktili", "Sindaktili", "Hemofilia"}, "Polidaktili", "Poli banyak daktil", {0,0,0,0}});
    exam.pool.push_back({"Kondisi sel darah merah sama sekali tak punya antigen golongan darah resesif?", {"Golongan O (i i)", "Golongan AB", "Golongan A", "Golongan B"}, "Golongan O (i i)", "Donor universal", {0,0,0,0}});
    exam.pool.push_back({"Penyakit eritrosit janin hancur diserang antibodi ibu beda darah positif?", {"Eritroblastosis Fetalis", "Thalassemia", "Hemofilia", "Polio"}, "Eritroblastosis Fetalis", "Ibu Rhesus negatif bahaya", {0,0,0,0}});
    exam.pool.push_back({"Pria mandul dada membesar kromosom seks lebih 47 XXY kelebihan X?", {"Klinefelter", "Down", "Turner", "Patau"}, "Klinefelter", "Feminim trisomi seks", {0,0,0,0}});
    exam.pool.push_back({"Wanita cebol leher sayap rahim tak tumbuh hilang satu X monosomi 45 XO?", {"Turner", "Edwards", "Klinefelter", "Down"}, "Turner", "Cuma punya satu X", {0,0,0,0}});
    exam.pool.push_back({"Sindrom trisomi kelebihan autosom kromosom nomor 21 muka mongoloid?", {"Down Syndrome", "Patau", "Klinefelter", "Turner"}, "Down Syndrome", "Kasus terumum", {0,0,0,0}});
    exam.pool.push_back({"Genotipe tulang jari sangat pendek letal mati di dalam kandungan (BB)?", {"Brakidaktili", "Polidaktili", "Sindaktili", "Akondro"}, "Brakidaktili", "Dominan mematikan", {0,0,0,0}});
    exam.pool.push_back({"Pewarisan sifat terpaut kromosom Y ayah menular murni ke seluruh anak pria?", {"Holandrik", "Terpaut X", "Autosom", "Sex-influenced"}, "Holandrik", "Bulu telinga lebat", {0,0,0,0}});
    exam.pool.push_back({"Bentuk sifat fisik yang terlihat akibat susunan gen dalam tubuh?", {"Fenotipe", "Genotipe", "Kariotipe", "Mutasi"}, "Fenotipe", "Ciri nampak", {0,0,0,0}});
    exam.pool.push_back({"Hukum genetika p2 + 2pq + q2 = 1 syarakat populasi seimbang diam tak mutasi?", {"Hardy-Weinberg", "Mendel 1", "Mendel 2", "Morgan"}, "Hardy-Weinberg", "Ekuilibrium frekuensi alel", {0,0,0,0}});
    exam.pool.push_back({"Anemia genetik herediter eritrosit gampang pecah hemoglobin abnormal?", {"Thalassemia", "Hemofilia", "Leukemia", "Albino"}, "Thalassemia", "Ras laut tengah/mediterania", {0,0,0,0}});
    exam.pool.push_back({"Sifat kebotakan pria aktif dipengaruhi hormon testosteron wanita aman?", {"Terpengaruh seks", "Terpaut X", "Terpaut Y", "Letal"}, "Terpengaruh seks", "Sex-influenced heterozigot beda", {0,0,0,0}});
    exam.pool.push_back({"Jari tangan kaki bayi menyatu gagal belah membran di kandungan?", {"Sindaktili", "Polidaktili", "Brakidaktili", "Anodontia"}, "Sindaktili", "Mirip katak bebek", {0,0,0,0}});
    exam.pool.push_back({"Kerusakan berat sel kerucut mata dunia total monokrom hitam putih abu?", {"Buta Warna Total", "Merah Hijau", "Miopi", "Astigmatisme"}, "Buta Warna Total", "Semua warna hilang", {0,0,0,0}});
    exam.pool.push_back({"Kromosom anafase lengket gagal berpisah ditarik benang sentriol?", {"Nondisjunction", "Pindah silang", "Translasi", "Inversi"}, "Nondisjunction", "Penyebab kelebihan kromosom trisomi", {0,0,0,0}});
    exam.pool.push_back({"Hukum pewarisan memisahnya alel secara bebas pada persilangan monohibrid?", {"Segregasi Mendel I", "Asortasi 2", "Linkage", "Mutasi"}, "Segregasi Mendel I", "Pisah gamet bebas murni", {0,0,0,0}});
    exam.pool.push_back({"Kelainan resesif terpaut X tanpa gigi rahang tulang benih tak tumbuh?", {"Anodontia", "Sindaktili", "Polidaktili", "Thalassemia"}, "Anodontia", "Ompong abadi", {0,0,0,0}});
    exam.pool.push_back({"Tukar silang antar lengan kromosom tetangga penyebab variasi genetik anak?", {"Pindah Silang (Crossing)", "Gagal Berpisah", "Delesi", "Letal"}, "Pindah Silang (Crossing)", "Terjadi di profase meiosis chiasma", {0,0,0,0}});
    exam.pool.push_back({"Dua gen jaraknya sangat dekat di kromosom sama pewarisan nempel bersamaan?", {"Tautan (Linkage)", "Pindah silang", "Gagal pisah", "Independen"}, "Tautan (Linkage)", "Lawan mandiri Morgan gen lalat", {0,0,0,0}});
}

extern "C" {
    EXPORT void init_game(unsigned int seed) { rng.seed(seed); init_data(); p = Player(); state = "menu"; }
    EXPORT void start_exam() { state = "explore"; }
    EXPORT void travel_to(int id) { if (state != "explore" && state != "shop") return; p.location = id; if (id == 1) { state = "shop"; } else { state = "combat"; m_currentHP = exam.maxHP; load_new_question(); } }
    EXPORT void exit_shop() { state = "explore"; p.location = 0; }
    EXPORT void buy_item(int id) { if (state != "shop") return; if (id == 0 && p.money >= 50) { p.money -= 50; p.currentHP += 50; if(p.currentHP > p.maxHP) p.currentHP = p.maxHP; } else if (id == 1 && p.money >= 100) { p.money -= 100; p.sp += 1; } else if (id == 2 && p.money >= 200) { p.money -= 200; p.baseAttack += 5; p.atkLevel++; } else if (id == 3 && p.money >= 300) { p.money -= 300; p.luckLevel++; } }
    EXPORT void use_skill(int id) { last_dmg_to_boss = 0; last_dmg_to_player = 0; last_hit_crit = false; if (state != "combat") return; if (id == 0 && p.sp >= 2) { p.sp -= 2; p.currentHP += (p.maxHP/2); if(p.currentHP > p.maxHP) p.currentHP = p.maxHP; } else if (id == 1 && p.sp >= 3) { p.sp -= 3; int h=0; for(int i=0; i<4; i++) { if(curQ.o[i] != curQ.a && !curQ.m[i]) { curQ.m[i]=true; h++; if(h>=2) break; } } } else if (id == 2 && p.sp >= 4) { p.sp -= 4; int dmg = 100; m_currentHP -= dmg; last_dmg_to_boss = dmg; q_answered++; load_new_question(); if(m_currentHP<=0) m_currentHP=1; } }
    EXPORT void check_answer(int idx) { last_dmg_to_boss = 0; last_dmg_to_player = 0; last_hit_crit = false; if (state != "combat") return; if (curQ.o[idx] == curQ.a) { int dmg = p.baseAttack; int critChance = 5 + (p.luckLevel * 5); if ((rng() % 100) < critChance) { dmg *= 2; last_hit_crit = true; } m_currentHP -= dmg; p.money += 30; last_dmg_to_boss = dmg; q_answered++; } else { p.currentHP -= 50; last_dmg_to_player = 50; } if (m_currentHP <= 0 || q_answered >= 200) { state = "victory"; final_score = (p.currentHP * 10) + (p.money * 5) + (q_answered * 100); } else if (p.currentHP <= 0) { state = "dead"; } else if (last_dmg_to_boss > 0) { load_new_question(); } }
    EXPORT void roll_gacha() { if(p.sp > 0) { p.sp--; p.atkLevel++; p.baseAttack += 10; p.currentHP = p.maxHP; } }
    
    EXPORT const char* get_state() { return state.c_str(); }
    EXPORT const char* get_q_text() { return curQ.q.c_str(); }
    EXPORT const char* get_q_opt(int i) { return curQ.o[i].c_str(); }
    EXPORT int is_opt_hidden(int i) { return curQ.m[i] ? 1 : 0; }
    EXPORT const char* get_boss_name() { return exam.name.c_str(); }
    EXPORT const char* get_q_hint() { return curQ.h.c_str(); }
    EXPORT int get_p_hp() { return p.currentHP; }
    EXPORT int get_p_sp() { return p.sp; }
    EXPORT int get_p_money() { return p.money; }
    EXPORT int get_p_luck() { return p.luckLevel; }
    EXPORT int get_m_hp() { return m_currentHP; }
    EXPORT int get_m_max_hp() { return exam.maxHP; }
    EXPORT int get_q_answered() { return q_answered; }
    EXPORT int get_last_dmg_boss() { return last_dmg_to_boss; }
    EXPORT int get_last_dmg_player() { return last_dmg_to_player; }
    EXPORT int is_last_crit() { return last_hit_crit ? 1 : 0; }
    EXPORT int get_final_score() { return final_score; }

    // Untuk memastikan tidak ada error saat kompilasi jika compile.bat lama masih menyangkut
    EXPORT int get_p_lvl() { return p.level; }
    EXPORT int get_p_atk() { return p.atkLevel; }
}

