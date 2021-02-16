#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <random>
#include <string.h>      //文字列の代入に使う
#include <bits/stdc++.h> //piの利用で必要(M_PI)
//complete
class ParticleInfo
{
public:
    uint32_t id;
    uint32_t type;
    /*position*/
    double posx;
    double posy;
    double posz;
    /*velocity*/
    double velx;
    double vely;
    double velz;
    /*結合*/
    uint32_t bond_pair[2];
    uint32_t bond_type[2];
    uint32_t nbond = 0;
    /*アングル*/
    uint32_t angle_pair[2][3];
    uint32_t angle_type[2];
    uint32_t nangle = 0;

    //sortを利用するために定義
    bool operator<(const ParticleInfo &another) const
    {
        //メンバ変数であるnum1で比較した結果を
        //この構造体の比較とする
        return id < another.id;
    }
};

class CenterOfGravity
{
public:
    /*position*/
    double x;
    double y;
    double z;
    /*数えた脂質粒子の個数*/
    int num;
    CenterOfGravity()
    {
        x = y = z = 0;
        num = 0;
    }
};

int main(int argc, char *argv[])
{
    std::vector<ParticleInfo> pinfo;
    ParticleInfo temp_info;
    /*
    
    
    
    
    座標の読み込みを行う．*/
    std::ifstream ifs0(argv[1]);
    if (!ifs0)
    {
        std::cerr << "error0" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    //いらないはじめの3行を捨てる．
    std::string delete_str[3];
    for (uint32_t i = 0; i < 3; i++)
    {
        std::getline(ifs0, delete_str[i]);
    }
    //ファイルの読み込み　粒子番号,粒子種は0から開始する．
    while (ifs0 >> temp_info.id >> temp_info.type >> temp_info.posx >> temp_info.posy >> temp_info.posz)
    {
        temp_info.id--;
        temp_info.type--;
        pinfo.push_back(temp_info);
    }
    ifs0.close();
    //はじめの文字列を読み込む
    double box_sx, box_sy, box_sz, box_ex, box_ey, box_ez, box_wt;
    sscanf(delete_str[0].c_str(), "'box_sx=%lf box_sy=%lf box_sz=%lf box_ex=%lf box_ey=%lf box_ez=%lf box_wt=%lf",
           &box_sx, &box_sy, &box_sz, &box_ex, &box_ey, &box_ez, &box_wt);
    //    std::cout <<std::setprecision(10)<< box_sx << " " << box_sy << " " << box_sz << " " << box_ex << " " << box_ey << " " << box_ez << " " << box_wt << std::endl;
    std::sort(pinfo.begin(), pinfo.end()); //classでオペレータを定義して利用している．
    /*




    ベシクルの重心を計算する．*/
    CenterOfGravity center_vesicle;
    for (int i = 0; i < pinfo.size(); i++)
    {
        if (pinfo.at(i).type != 2)
        {
            center_vesicle.x += pinfo.at(i).posx;
            center_vesicle.y += pinfo.at(i).posy;
            center_vesicle.z += pinfo.at(i).posz;
            center_vesicle.num++;
        }
    }
    center_vesicle.x /= (double)center_vesicle.num;
    center_vesicle.y /= (double)center_vesicle.num;
    center_vesicle.z /= (double)center_vesicle.num;
    std::cout << center_vesicle.x << " " << center_vesicle.y << " " << center_vesicle.z << std::endl;
    /*







    同系方向の粒子種を数える*/
    class Gr
    {
    public:
        double distance = 0;
        uint32_t number_of_0 = 0;
        uint32_t number_of_1 = 0;
        uint32_t number_of_2 = 0;
    };
    /*自分で決めるパラメータ*/
    double dr = 0.001; //同系方向の円形ボックスの厚み．この中にいくつ粒子が入っているか数える．
    /*確定しているもの*/
    double search_r = 0.0;
    double r_now;
    uint32_t num_of_gr = (uint32_t)(box_ex - box_sx) / dr + 1;

    std::vector<Gr> gr_info(num_of_gr);
    //    Gr temp_gr_info;
    for (int i; i < pinfo.size(); i++)
    {
        r_now = pow(pow(pinfo.at(i).posx - center_vesicle.x, 2.0) + pow(pinfo.at(i).posy - center_vesicle.y, 2.0) + pow(pinfo.at(i).posz - center_vesicle.z, 2.0), 0.5);
        if (pinfo.at(i).type == 0)
            gr_info.at((uint32_t)(r_now / dr)).number_of_0++;
        else if (pinfo.at(i).type == 1)
            gr_info.at((uint32_t)(r_now / dr)).number_of_1++;
        else if (pinfo.at(i).type == 2)
            gr_info.at((uint32_t)(r_now / dr)).number_of_2++;
    }
//密度で割る必要あり
    /*







ファイルの出力*/

    //vel_file
    FILE *fpo1;
    fpo1 = fopen(argv[2], "w");
    if (fpo1 == NULL)
    {
        printf("ERROR_file_output\n");
        return -1;
    }
    for (int i = 0; i < gr_info.size(); i++)
    {
        fprintf(fpo1, "%lf   %d   %d   %d \n",
                dr*i,
                gr_info.at(i).number_of_0,
                gr_info.at(i).number_of_1,
                gr_info.at(i).number_of_2);
    }
    fclose(fpo1);
#if 0
    /*





    */
    //bond_fileここからはofstream記法で出力する．
    std::ofstream fpo2(argv[7], std::ios::out);
    for (int i = 0; i < double_vesicle_pinfo.size(); i++)
    {
        //"&&"条件で，double_vesicle_pinfo.at(i).nbond == 1のときに重複して出力してしまうのを防ぐ．
        if (double_vesicle_pinfo.at(i).nbond == 1 && double_vesicle_pinfo.at(i).id < double_vesicle_pinfo.at(i).bond_pair[0])
        {
            fpo2 << double_vesicle_pinfo.at(i).id + 1 << "   " << double_vesicle_pinfo.at(i).bond_pair[0] + 1 << "   " << double_vesicle_pinfo.at(i).bond_type[0] + 1 << std::endl;
        }
        else if (double_vesicle_pinfo.at(i).nbond == 2)
        {
            fpo2 << double_vesicle_pinfo.at(i).id + 1 << "   " << double_vesicle_pinfo.at(i).bond_pair[1] + 1 << "   " << double_vesicle_pinfo.at(i).bond_type[0] + 1 << std::endl;
        }
    }
    fpo2.close();
    /*





    */
    //bond_file ofstream記法で出力する．
    //angle_file
    std::ofstream fpo3(argv[8], std::ios::out);
    for (int i = 0; i < double_vesicle_pinfo.size(); i++)
    {
        if (double_vesicle_pinfo.at(i).nangle == 1)
        {
            fpo3 << double_vesicle_pinfo.at(i).angle_pair[0][0] + 1 << "   " << double_vesicle_pinfo.at(i).angle_pair[0][1] + 1 << "   " << double_vesicle_pinfo.at(i).angle_pair[0][2] + 1 << "   " << double_vesicle_pinfo.at(i).angle_type[0] + 1 << std::endl;
        }
    }
    fpo3.close();

    /*





    */

    for (int i = 0; i < double_vesicle_pinfo.size(); i++)
    {
        if (double_vesicle_pinfo.at(i).type + 1 == 3)
            num_water++;
        else
            num_lipid++;
    }
    num = num_water + num_lipid;
    rho = num / (box_size_x * box_size_y * box_size_z);
    std::string filename0 = argv[9];
    std::ofstream writing_file0;
    writing_file0.open(filename0, std::ios::out);
    writing_file0 << "自分で決めるパラメータ" << std::endl;
    writing_file0 << "box_x_y = " << box_size_y / box_size_x << " (ボックスサイズのxが1としたときのyの比) " << std::endl;
    writing_file0 << "box_x_z = " << box_size_z / box_size_x << " (ボックスサイズのxが1としたときのzの比) " << std::endl;
    writing_file0 << std::endl;
    writing_file0 << "確定するパラメータ" << std::endl;
    writing_file0 << "num_water = " << num_water << " (水粒子の数) " << std::endl;
    writing_file0 << "num_lipid = " << num_lipid << " (脂質を構成する粒子の数) " << std::endl;
    writing_file0 << "num = " << num << " (すべての粒子数) " << std::endl;
    writing_file0 << "rho = " << rho << " (密度) " << std::endl;
    writing_file0 << "box_size_x = " << box_size_x << " (ボックスサイズxyz三辺が1：1：1の場合, box_size_x = box_size_y = box_size_z. ) " << std::endl;
    writing_file0 << "box_size_y = " << box_size_y << std::endl;
    writing_file0 << "box_size_z = " << box_size_z << std::endl;
    writing_file0 << std::endl;
    writing_file0 << "water : lipid = " << (double)num_water / (double)num * 100.0 << " : " << (double)num_lipid / (double)num * 100.0 << std::endl;
    writing_file0.close();
#endif

    return 0;
}