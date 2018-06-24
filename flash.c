/* 
 * Contributors: Youngjae Kim (youkim@cse.psu.edu)
 *               Aayush Gupta (axg354@cse.psu.edu)
 * 
 * In case if you have any doubts or questions, kindly write to: youkim@cse.psu.edu 
 *   
 * This source code provides page-level FTL scheme. 
 * 
 * Acknowledgement: We thank Jeong Uk Kang by sharing the initial version 
 * of sector-level FTL source code. 
 * 
 */

#include <stdlib.h>
#include <string.h>
#include "flash.h"
#include "ssd_interface.h"

_u32 nand_blk_num,nand_SLC_blk_num,nand_MLC_blk_num, min_fb_num,min_wfb_num,min_cfb_num;//用在opm_gc_cost_benefit()
_u8  pb_size;
struct nand_blk_info *nand_blk;
struct SLC_nand_blk_info *SLC_nand_blk;
struct MLC_nand_blk_info *MLC_nand_blk;
struct SLC_nand_blk_info *head;
struct SLC_nand_blk_info *tail;
struct SLC_nand_blk_info *hot_head;
struct SLC_nand_blk_info *hot_tail;
struct SLC_nand_blk_info *cold_head;
struct SLC_nand_blk_info *cold_tail;
int MIN_ERASE;

/**************** NAND STAT **********************/
void nand_stat(int option)
{ 
    switch(option){

      case PAGE_READ:
        stat_read_num++;
        flash_read_num++;
        break;

      case PAGE_WRITE :
        stat_write_num++;
        flash_write_num++;
        break;

      case OOB_READ:
        stat_oob_read_num++;
        flash_oob_read_num++;
        break;

      case OOB_WRITE:
        stat_oob_write_num++;
        flash_oob_write_num++;
        break;

      case BLOCK_ERASE:
        stat_erase_num++;
        flash_erase_num++;
        break;

      case GC_PAGE_READ:
        stat_gc_read_num++;
        flash_gc_read_num++;
        break;
    
      case GC_PAGE_WRITE:
        stat_gc_write_num++;
        flash_gc_write_num++;
        break;
      case SLC_PAGE_READ:
        SLC_stat_read_num++;
        SLC_flash_read_num++;
        break;

      case SLC_PAGE_WRITE :
        SLC_stat_write_num++;
        SLC_flash_write_num++;
        break;

      case SLC_OOB_READ:
        SLC_stat_oob_read_num++;
        SLC_flash_oob_read_num++;
        break;

      case SLC_OOB_WRITE:
        SLC_stat_oob_write_num++;
        SLC_flash_oob_write_num++;
        break;

      case SLC_BLOCK_ERASE:
        SLC_stat_erase_num++;
        SLC_flash_erase_num++;
        break;

      case SLC_GC_PAGE_READ:
        SLC_stat_gc_read_num++;
        SLC_flash_gc_read_num++;
        break;
    
      case SLC_GC_PAGE_WRITE:
        SLC_stat_gc_write_num++;
        SLC_flash_gc_write_num++;
        break;
      case MLC_PAGE_READ:
        MLC_stat_read_num++;
        MLC_flash_read_num++;
        break;

      case MLC_PAGE_WRITE :
        MLC_stat_write_num++;
        MLC_flash_write_num++;
        break;

      case MLC_OOB_READ:
        MLC_stat_oob_read_num++;
        MLC_flash_oob_read_num++;
        break;

      case MLC_OOB_WRITE:
        MLC_stat_oob_write_num++;
        MLC_flash_oob_write_num++;
        break;

      case MLC_BLOCK_ERASE:
        MLC_stat_erase_num++;
        MLC_flash_erase_num++;
        break;

      case MLC_GC_PAGE_READ:
        MLC_stat_gc_read_num++;
        MLC_flash_gc_read_num++;
        break;
    
      case MLC_GC_PAGE_WRITE:
        MLC_stat_gc_write_num++;
        MLC_flash_gc_write_num++;
        break;

      default: 
        ASSERT(0);
        break;
    }
}

void nand_stat_reset()
{
  stat_read_num = stat_write_num = stat_erase_num = 0;
  stat_gc_read_num = stat_gc_write_num = 0;
  stat_oob_read_num = stat_oob_write_num = 0;
  SLC_stat_read_num = SLC_stat_write_num = SLC_stat_erase_num = 0;
  SLC_stat_gc_read_num = SLC_stat_gc_write_num = 0;
  SLC_stat_oob_read_num = SLC_stat_oob_write_num = 0;
  MLC_stat_read_num = MLC_stat_write_num = MLC_stat_erase_num = 0;
  MLC_stat_gc_read_num = MLC_stat_gc_write_num = 0;
  MLC_stat_oob_read_num = MLC_stat_oob_write_num = 0;
}

void nand_stat_print(FILE *outFP)
{
  fprintf(outFP, "\n");
  fprintf(outFP, "FLASH STATISTICS\n");
  fprintf(outFP, "------------------------------------------------------------\n");
  fprintf(outFP, " Page read (#): %8u   ", stat_read_num);
  fprintf(outFP, " Page write (#): %8u   ", stat_write_num);
  fprintf(outFP, " Block erase (#): %8u\n", stat_erase_num);
//  fprintf(outFP, " OOREAD  %8u   ", stat_oob_read_num);
//  fprintf(outFP, " OOWRITE %8u\n", stat_oob_write_num);
  fprintf(outFP, " GC page read (#): %8u   ", stat_gc_read_num);
  fprintf(outFP, " GC page write (#): %8u\n", stat_gc_write_num);
  fprintf(outFP, "------------------------------------------------------------\n");
  fprintf(outFP, "------------------------------------------------------------\n");
  fprintf(outFP, " SLC_Page read (#): %8u   ", SLC_stat_read_num);
  fprintf(outFP, " SLC_Page write (#): %8u   ", SLC_stat_write_num);
  fprintf(outFP, " SLC_Block erase (#): %8u\n", SLC_stat_erase_num);
//  fprintf(outFP, " OOREAD  %8u   ", stat_oob_read_num);
//  fprintf(outFP, " OOWRITE %8u\n", stat_oob_write_num);
  fprintf(outFP, " SLC_GC page read (#): %8u   ", SLC_stat_gc_read_num);
  fprintf(outFP, " SLC_GC page write (#): %8u\n", SLC_stat_gc_write_num);
  fprintf(outFP, "------------------------------------------------------------\n");
  fprintf(outFP, "------------------------------------------------------------\n");
  fprintf(outFP, " MLC_Page read (#): %8u   ", MLC_stat_read_num);
  fprintf(outFP, " MLC_Page write (#): %8u   ", MLC_stat_write_num);
  fprintf(outFP, " MLC_Block erase (#): %8u\n", MLC_stat_erase_num);
//  fprintf(outFP, " OOREAD  %8u   ", stat_oob_read_num);
//  fprintf(outFP, " OOWRITE %8u\n", stat_oob_write_num);
  fprintf(outFP, " MLC__GC page read (#): %8u   ", MLC_stat_gc_read_num);
  fprintf(outFP, " MLC_GC page write (#): %8u\n", MLC_stat_gc_write_num);
  fprintf(outFP, "------------------------------------------------------------\n");
}

/**************** NAND INIT **********************/
// 初始化对应的结构体，MLC和SLC都是结构体数组
int nand_init (_u32 blk_num,_u32 SLC_blk_num,_u32 MLC_blk_num, _u8 min_free_blk_num)
{
  // 有个地方难以理解，既然分类SLC和MLC为什么还要初始化对应的一个nand_blk?数组？
  _u32 blk_no;
  int i;
// 先检查指针是否为NULL做下释放
  nand_end();

  nand_blk = (struct nand_blk_info *)malloc(sizeof (struct nand_blk_info) * blk_num);
  SLC_nand_blk = (struct SLC_nand_blk_info *)malloc(sizeof (struct SLC_nand_blk_info) * SLC_blk_num);
  MLC_nand_blk = (struct MLC_nand_blk_info *)malloc(sizeof (struct MLC_nand_blk_info) * MLC_blk_num);
  if ((nand_blk == NULL)||(SLC_nand_blk == NULL)||(MLC_nand_blk == NULL)) 
  {
    return -1;
  }
  // 循环队列SLC的处理用到了head,tail,cold_head,cold_tial
  head=tail=&SLC_nand_blk[0]; 
//  hot_head=hot_tail=&SLC_nand_blk[512]; 
// 这个块号为什么是3072？？？
  cold_head=cold_tail=&SLC_nand_blk[3072];
  memset(nand_blk, 0xFF, sizeof (struct nand_blk_info) * blk_num);
  memset(SLC_nand_blk, 0xFF, sizeof (struct SLC_nand_blk_info) * SLC_blk_num);
  memset(MLC_nand_blk, 0xFF, sizeof (struct MLC_nand_blk_info) * MLC_blk_num);

  nand_blk_num = blk_num;
  nand_SLC_blk_num = SLC_blk_num;
  nand_MLC_blk_num = MLC_blk_num;
  pb_size = 1;
  min_fb_num = min_free_blk_num;
  for (blk_no = 0; blk_no < nand_SLC_blk_num; blk_no++) {
    SLC_nand_blk[blk_no].state.free = 1;
    SLC_nand_blk[blk_no].state.ec = 0;
    SLC_nand_blk[blk_no].fpc = S_SECT_NUM_PER_BLK;
    SLC_nand_blk[blk_no].ipc = 0;
    SLC_nand_blk[blk_no].lwn = -1;


    for(i = 0; i<S_SECT_NUM_PER_BLK; i++){
      SLC_nand_blk[blk_no].sect[i].free = 1;
      SLC_nand_blk[blk_no].sect[i].valid = 0;
      SLC_nand_blk[blk_no].sect[i].lsn = -1;
    }

    for(i = 0; i < S_PAGE_NUM_PER_BLK; i++){
      SLC_nand_blk[blk_no].page_status[i] = -1; // 0: data, 1: map table
    }
  }
  for (blk_no =0 ; blk_no < nand_MLC_blk_num; blk_no++) {
    MLC_nand_blk[blk_no].state.free = 1;
    MLC_nand_blk[blk_no].state.ec = 0;
    MLC_nand_blk[blk_no].fpc = M_SECT_NUM_PER_BLK;
    MLC_nand_blk[blk_no].ipc = 0;
    MLC_nand_blk[blk_no].lwn = -1;


    for(i = 0; i<M_SECT_NUM_PER_BLK; i++){
      MLC_nand_blk[blk_no].sect[i].free = 1;
      MLC_nand_blk[blk_no].sect[i].valid = 0;
      MLC_nand_blk[blk_no].sect[i].lsn = -1;
    }

    for(i = 0; i < M_PAGE_NUM_PER_BLK; i++){
      MLC_nand_blk[blk_no].page_status[i] = -1; // 0: data, 1: map table
    }
  }
  free_blk_num = nand_blk_num;
  free_SLC_blk_num = 3072;
//  free_hot_blk_num = 3072;
  free_cold_blk_num = 1024;
  free_MLC_blk_num = nand_MLC_blk_num;
  free_blk_idx =0;
  nand_stat_reset();
  
  return 0;
}

/**************** NAND END **********************/
void nand_end ()
{
  nand_blk_num = 0;
  if (nand_blk != NULL) {
    nand_blk = NULL;
  }
  if (SLC_nand_blk != NULL) {
    SLC_nand_blk = NULL;
  }
  if (MLC_nand_blk != NULL) {
    MLC_nand_blk = NULL;
  }
}

/**************** NAND OOB READ **********************/
// nand_oob_read通过nand_blk[pbn].sect[pin].free 和nand_blk[pbn].sect[pin].valid结合判断页的状态（有效1,无效-1，空闲 0）
// 会增加一次统计变量oob_read
int nand_oob_read(_u32 psn)
{
  blk_t pbn = BLK_F_SECT(psn);	// physical block number	
  _u16  pin = IND_F_SECT(psn);	// page index (within the block), here page index is the same as sector index
  _u16  i, valid_flag = 0;

  ASSERT(pbn < nand_blk_num);	// pbn shouldn't exceed max nand block number 
// 从页的起始扇区到结束山区的连续区域进行检测，只要一个满足直接跳出
  for (i = 0; i < SECT_NUM_PER_PAGE; i++) {
    if(nand_blk[pbn].sect[pin + i].free == 0){

      if(nand_blk[pbn].sect[pin + i].valid == 1){
        valid_flag = 1;//非空闲且有效
        break;
      }
      else{
        valid_flag = -1;//非空闲且无效
        break;
      }
    }
    else{
      valid_flag = 0;//空闲且有效
      break;
    }
  }

  nand_stat(OOB_READ);
  
  return valid_flag;
}

// 和上面nand_oob_read的实现一模一样
int SLC_nand_oob_read(_u32 psn)
{
  blk_t pbn = S_BLK_F_SECT(psn);	// physical block number	
  _u16  pin = S_IND_F_SECT(psn);	// page index (within the block), here page index is the same as sector index
  _u16  i, valid_flag = 0;

  ASSERT(pbn < nand_SLC_blk_num);	// pbn shouldn't exceed max nand block number 

  for (i = 0; i < S_SECT_NUM_PER_PAGE; i++) {
    if(SLC_nand_blk[pbn].sect[pin + i].free == 0){

      if(SLC_nand_blk[pbn].sect[pin + i].valid == 1){
        valid_flag = 1;
        break;
      }
      else{
        valid_flag = -1;
        break;
      }
    }
    else{
      valid_flag = 0;
      break;
    }
  }

  nand_stat(SLC_OOB_READ);
  
  return valid_flag;
}
int MLC_nand_oob_read(_u32 psn)
{
  blk_t pbn = M_BLK_F_SECT(psn);	// physical block number	
  _u16  pin = M_IND_F_SECT(psn);	// page index (within the block), here page index is the same as sector index
  _u16  i, valid_flag = 0;

  ASSERT(pbn < nand_MLC_blk_num);	// pbn shouldn't exceed max nand block number 

  for (i = 0; i < M_SECT_NUM_PER_PAGE; i++) {
    if(MLC_nand_blk[pbn].sect[pin + i].free == 0){

      if(MLC_nand_blk[pbn].sect[pin + i].valid == 1){
        valid_flag = 1;
        break;
      }
      else{
        valid_flag = -1;
        break;
      }
    }
    else{
      valid_flag = 0;
      break;
    }
  }

  nand_stat(MLC_OOB_READ);
  
  return valid_flag;
}

void break_point()
{
  printf("break point\n");
}

/**************** NAND PAGE READ **********************/
// 调用该函数分为垃圾回收读（拷贝有效页）和非垃圾回收读
// 传入的lsns是个数组 lsns[SECT_NUM_PER_PAGE]，一个页的扇区内容（nand_blk[pbn].sect[pin+i].lsn,0<i<SECT_NUM_PER_PAGE）
// isGC =1 处于垃圾回收状态
_u8 nand_page_read(_u32 psn, _u32 *lsns, _u8 isGC)
{
  blk_t pbn = BLK_F_SECT(psn);	// physical block number	
  _u16  pin = IND_F_SECT(psn);	// page index (within the block), here page index is the same as sector index
  _u16  i,j, valid_sect_num = 0;

  if(pbn >= nand_blk_num){
    printf("psn: %d, pbn: %d, nand_blk_num: %d\n", psn, pbn, nand_blk_num);
  }

  ASSERT(OFF_F_SECT(psn) == 0);
  // 这段代码中的8848块遍历，8848的值确定死有问题？
  if(nand_blk[pbn].state.free != 0) {
    for( i =0 ; i < 8448 ; i++){
      for(j =0; j < 256;j++){
        if(nand_blk[i].sect[j].lsn == lsns[0]){
          printf("blk = %d",i);
          break;
        }
      }
    }
  }

// 读的块必须有内容，即nand_blk[pbn].sect[pin].lsn有值
  ASSERT(nand_blk[pbn].state.free == 0);	// block should be written with something

  if (isGC == 1) {
    for (i = 0; i < SECT_NUM_PER_PAGE; i++) {

      if((nand_blk[pbn].sect[pin + i].free == 0) &&
         (nand_blk[pbn].sect[pin + i].valid == 1)) {
        lsns[valid_sect_num] = nand_blk[pbn].sect[pin + i].lsn;
        valid_sect_num++;
      }
    }
// 错误调试，一个页中的扇区的有效应该都一样，valid_sect_num应该为4
    if(valid_sect_num == 3){
      for(i = 0; i<SECT_NUM_PER_PAGE; i++){
        printf("pbn: %d, pin %d: %d, free: %d, valid: %d\n", 
            pbn, i, pin+i, nand_blk[pbn].sect[pin+i].free, nand_blk[pbn].sect[pin+i].valid);

      }
      exit(0);
    }
// isGC==2的状态？翻译页的回收？可以要求一个页的扇区的状态不连续？
  } else if (isGC == 2) {
    for (i = 0; i < SECT_NUM_PER_PAGE; i++) {
      if (lsns[i] != -1) {
        if (nand_blk[pbn].sect[pin + i].free == 0 &&
            nand_blk[pbn].sect[pin + i].valid == 1) {
          ASSERT(nand_blk[pbn].sect[pin + i].lsn == lsns[i]);
          valid_sect_num++;
        } else {
          lsns[i] = -1;
        }
      }
    }
  } 
// 正常地读取数据页，但是该情况下的lsns[i]内有扇区的值，作为核算的数据进来
  else { // every sector should be "valid", "not free"   
    for (i = 0; i < SECT_NUM_PER_PAGE; i++) {
      if (lsns[i] != -1) {

        ASSERT(nand_blk[pbn].sect[pin + i].free == 0);
        ASSERT(nand_blk[pbn].sect[pin + i].valid == 1);
        ASSERT(nand_blk[pbn].sect[pin + i].lsn == lsns[i]);
        valid_sect_num++;
      }
      else{
        printf("lsns[%d]: %d shouldn't be -1\n", i, lsns[i]);
        exit(0);
      }
    }
  }
  // 统计数据页的读操作次数
  if (isGC) {
    if (valid_sect_num > 0) {
      nand_stat(GC_PAGE_READ);
    }
  } else {
    nand_stat(PAGE_READ);
  }
  // 最后返回的值一般没问题是4，isGC==2返回的不一定是4
  return valid_sect_num;
}

// 结构和上面的nand_page_read是一样的
_u8 SLC_nand_page_read(_u32 psn, _u32 *lsns, _u8 isGC)
{
  blk_t pbn = S_BLK_F_SECT(psn);	// physical block number	
  _u16  pin = S_IND_F_SECT(psn);	// page index (within the block), here page index is the same as sector index
  _u16  i,j, valid_sect_num = 0;

  if(pbn >= nand_SLC_blk_num){
    printf("psn: %d, pbn: %d, nand_SLC_blk_num: %d\n", psn, pbn, nand_SLC_blk_num);
  }

  ASSERT(S_OFF_F_SECT(psn) == 0);
  if(SLC_nand_blk[pbn].state.free != 0) {
    for( i =0 ; i < 4224 ; i++){
      for(j =0; j < 256;j++){
        if(SLC_nand_blk[i].sect[j].lsn == lsns[0]){
          printf("blk = %d",i);
          break;
        }
      }
    }
  }

  ASSERT(SLC_nand_blk[pbn].state.free == 0);	// block should be written with something

  if (isGC == 1) {
    for (i = 0; i < S_SECT_NUM_PER_PAGE; i++) {

      if((SLC_nand_blk[pbn].sect[pin + i].free == 0) &&
         (SLC_nand_blk[pbn].sect[pin + i].valid == 1)) {
        lsns[valid_sect_num] = SLC_nand_blk[pbn].sect[pin + i].lsn;
        valid_sect_num++;
      }
    }

    if(valid_sect_num == 3){
      for(i = 0; i<S_SECT_NUM_PER_PAGE; i++){
        printf("pbn: %d, pin %d: %d, free: %d, valid: %d\n", 
            pbn, i, pin+i, SLC_nand_blk[pbn].sect[pin+i].free, SLC_nand_blk[pbn].sect[pin+i].valid);

      }
      exit(0);
    }

  } else if (isGC == 2) {
    for (i = 0; i < S_SECT_NUM_PER_PAGE; i++) {
      if (lsns[i] != -1) {
        if (SLC_nand_blk[pbn].sect[pin + i].free == 0 &&
            SLC_nand_blk[pbn].sect[pin + i].valid == 1) {
          ASSERT(SLC_nand_blk[pbn].sect[pin + i].lsn == lsns[i]);
          valid_sect_num++;
        } else {
          lsns[i] = -1;
        }
      }
    }
  } 

  else { // every sector should be "valid", "not free"   
    for (i = 0; i < S_SECT_NUM_PER_PAGE; i++) {
      if (lsns[i] != -1) {

        ASSERT(SLC_nand_blk[pbn].sect[pin + i].free == 0);
        ASSERT(SLC_nand_blk[pbn].sect[pin + i].valid == 1);
        ASSERT(SLC_nand_blk[pbn].sect[pin + i].lsn == lsns[i]);
        valid_sect_num++;
      }
      else{
        printf("lsns[%d]: %d shouldn't be -1\n", i, lsns[i]);
        exit(0);
      }
    }
  }
  
  if (isGC) {
    if (valid_sect_num > 0) {
      nand_stat(SLC_GC_PAGE_READ);
    }
  } else {
    nand_stat(SLC_PAGE_READ);
  }
  
  return valid_sect_num;
}
// 结构和nand_page_read 是一样的，但块大小和页大小会影响到块包含扇区的大小
_u8 MLC_nand_page_read(_u32 psn, _u32 *lsns, _u8 isGC)
{
  blk_t pbn = M_BLK_F_SECT(psn);	// physical block number	
  _u16  pin = M_IND_F_SECT(psn);	// page index (within the block), here page index is the same as sector index
  _u16  i,j, valid_sect_num = 0;

  if(pbn >= nand_MLC_blk_num){
    printf("psn: %d, pbn: %d, nand_MLC_blk_num: %d\n", psn, pbn, nand_MLC_blk_num);
  }

  ASSERT(M_OFF_F_SECT(psn) == 0);
  if(MLC_nand_blk[pbn].state.free != 0) {
    for( i =0 ; i < 4224 ; i++){
      for(j =0; j < 1024;j++){
        if(MLC_nand_blk[i].sect[j].lsn == lsns[0]){
          printf("blk = %d",i);
          break;
        }
      }
    }
  }

  ASSERT(MLC_nand_blk[pbn].state.free == 0);	// block should be written with something

  if (isGC == 1) {
    for (i = 0; i < M_SECT_NUM_PER_PAGE; i++) {

      if((MLC_nand_blk[pbn].sect[pin + i].free == 0) &&
         (MLC_nand_blk[pbn].sect[pin + i].valid == 1)) {
        lsns[valid_sect_num] = MLC_nand_blk[pbn].sect[pin + i].lsn;
        valid_sect_num++;
      }
    }

    if(valid_sect_num == 7){
      for(i = 0; i<M_SECT_NUM_PER_PAGE; i++){
        printf("pbn: %d, pin %d: %d, free: %d, valid: %d\n", 
            pbn, i, pin+i, MLC_nand_blk[pbn].sect[pin+i].free, MLC_nand_blk[pbn].sect[pin+i].valid);

      }
      exit(0);
    }

  } else if (isGC == 2) {
    for (i = 0; i < M_SECT_NUM_PER_PAGE; i++) {
      if (lsns[i] != -1) {
        if (MLC_nand_blk[pbn].sect[pin + i].free == 0 &&
            MLC_nand_blk[pbn].sect[pin + i].valid == 1) {
          ASSERT(MLC_nand_blk[pbn].sect[pin + i].lsn == lsns[i]);
          valid_sect_num++;
        } else {
          lsns[i] = -1;
        }
      }
    }
  } 

  else { // every sector should be "valid", "not free"   
    for (i = 0; i < M_SECT_NUM_PER_PAGE; i++) {
      if (lsns[i] != -1) {

        ASSERT(MLC_nand_blk[pbn].sect[pin + i].free == 0);
        ASSERT(MLC_nand_blk[pbn].sect[pin + i].valid == 1);
        ASSERT(MLC_nand_blk[pbn].sect[pin + i].lsn == lsns[i]);
        valid_sect_num++;
      }
      else{
        printf("lsns[%d]: %d shouldn't be -1\n", i, lsns[i]);
        exit(0);
      }
    }
  }
  
  if (isGC) {
    if (valid_sect_num > 0) {
      nand_stat(MLC_GC_PAGE_READ);
    }
  } else {
    nand_stat(MLC_PAGE_READ);
  }
  
  return valid_sect_num;
}

/**************** NAND PAGE WRITE **********************/
// 关于数据页写入的区分有翻译页更新写入，数据页写入
// map_flag=2-->翻译页   -->更新nand_blk[pbn].page_stast[pin/M_SESCT_NUM_PER_PAGE]=1
// map_flag=2-->数据页   -->更新nand_blk[pbn].page_stast[pin/M_SESCT_NUM_PER_PAGE]=2
// psn是确定要写入的扇区位置，lsn[M_SECT_NUM_PER_PAGE]是要写入的东西
// 具体的写入又分为数据页更新的次数，翻译页的更新次数，其实这里就可以添加自己的统计变量查看翻译页更新的次数
_u8 MLC_nand_page_write(_u32 psn, _u32 *lsns, _u8 isGC, int map_flag)
{
  blk_t pbn = M_BLK_F_SECT(psn);	// physical block number with psn
  _u16  pin = M_IND_F_SECT(psn);	// sector index, page index is the same as sector index 
  int i, valid_sect_num = 0;
  
  
  if(pbn >= nand_MLC_blk_num){
    printf("break !\n");
  }

  ASSERT(pbn < nand_MLC_blk_num);
  ASSERT(M_OFF_F_SECT(psn) == 0);
// 更新对应的page状态
  if(map_flag == 2) {
        MLC_nand_blk[pbn].page_status[pin/M_SECT_NUM_PER_PAGE] = 1; // 1 for map table
  }
  else{
    MLC_nand_blk[pbn].page_status[pin/M_SECT_NUM_PER_PAGE] = 0; // 0 for data 
  }
// 往nand_blk[pbn].sect[pin].lsn添入值，改变nand_blk[pbn].sect[pin].free和valid的状态
  for (i = 0; i <M_SECT_NUM_PER_PAGE; i++) {

    if (lsns[i] != -1) {

      if(MLC_nand_blk[pbn].state.free == 1) {
        printf("blk num = %d",pbn);
      }

      ASSERT(MLC_nand_blk[pbn].sect[pin + i].free == 1);
      
      MLC_nand_blk[pbn].sect[pin + i].free = 0;			
      MLC_nand_blk[pbn].sect[pin + i].valid = 1;			
      MLC_nand_blk[pbn].sect[pin + i].lsn = lsns[i];	
      MLC_nand_blk[pbn].fpc--;  
      MLC_nand_blk[pbn].lwn = pin + i;	
      valid_sect_num++;
    }
    else{
      printf("lsns[%d] do not have any lsn\n", i);
    }
  }
  
  ASSERT(MLC_nand_blk[pbn].fpc >= 0);
// 根据isGC的状态区分统计量的更新
  if (isGC) {
    nand_stat(MLC_GC_PAGE_WRITE);
  } else {
    nand_stat(MLC_PAGE_WRITE);
  }
// 返回值写入的lsn[M_SECT_NUM_PER_PAGE]非负值
  return valid_sect_num;
}


// SLC的写入更新跟MLC一样的函数，但是SLC实际采用的是PPFTL，所以map_flag一般为1，方便后期的拓展
_u8 SLC_nand_page_write(_u32 psn, _u32 *lsns, _u8 isGC, int map_flag)
{
  blk_t pbn = S_BLK_F_SECT(psn);	// physical block number with psn
  _u16  pin = S_IND_F_SECT(psn);	// sector index, page index is the same as sector index 
  int i, valid_sect_num = 0;


  if(pbn >= nand_SLC_blk_num){
    printf("break !\n");
  }

  ASSERT(pbn < nand_SLC_blk_num);
  ASSERT(S_OFF_F_SECT(psn) == 0);

  if(map_flag == 2) {
        SLC_nand_blk[pbn].page_status[pin/S_SECT_NUM_PER_PAGE] = 1; // 1 for map table
  }
  else{
    SLC_nand_blk[pbn].page_status[pin/S_SECT_NUM_PER_PAGE] = 0; // 0 for data 
  }

  for (i = 0; i <S_SECT_NUM_PER_PAGE; i++) {

    if (lsns[i] != -1) {

      if(SLC_nand_blk[pbn].state.free == 1) {
        printf("blk num = %d",pbn);
      }

      ASSERT(SLC_nand_blk[pbn].sect[pin + i].free == 1);
      
      SLC_nand_blk[pbn].sect[pin + i].free = 0;			
      SLC_nand_blk[pbn].sect[pin + i].valid = 1;			
      SLC_nand_blk[pbn].sect[pin + i].lsn = lsns[i];	
      SLC_nand_blk[pbn].fpc--;  
      SLC_nand_blk[pbn].lwn = pin + i;	
      valid_sect_num++;
    }
    else{
      printf("lsns[%d] do not have any lsn\n", i);
    }
  }
  
  ASSERT(SLC_nand_blk[pbn].fpc >= 0);

  if (isGC) {
    nand_stat(SLC_GC_PAGE_WRITE);
  } else {
    nand_stat(SLC_PAGE_WRITE);
  }

  return valid_sect_num;
}

_u8 nand_page_write(_u32 psn, _u32 *lsns, _u8 isGC, int map_flag)
{
  blk_t pbn = BLK_F_SECT(psn);	// physical block number with psn
  _u16  pin = IND_F_SECT(psn);	// sector index, page index is the same as sector index 
  int i, valid_sect_num = 0;


  if(pbn >= nand_blk_num){
    printf("break !\n");
  }

  ASSERT(pbn < nand_blk_num);
  ASSERT(OFF_F_SECT(psn) == 0);

  if(map_flag == 2) {
        nand_blk[pbn].page_status[pin/SECT_NUM_PER_PAGE] = 1; // 1 for map table
  }
  else{
    nand_blk[pbn].page_status[pin/SECT_NUM_PER_PAGE] = 0; // 0 for data 
  }

  for (i = 0; i <SECT_NUM_PER_PAGE; i++) {

    if (lsns[i] != -1) {

      if(nand_blk[pbn].state.free == 1) {
        printf("blk num = %d",pbn);
      }

      ASSERT(nand_blk[pbn].sect[pin + i].free == 1);
      
      nand_blk[pbn].sect[pin + i].free = 0;			
      nand_blk[pbn].sect[pin + i].valid = 1;			
      nand_blk[pbn].sect[pin + i].lsn = lsns[i];	
      nand_blk[pbn].fpc--;  
      nand_blk[pbn].lwn = pin + i;	
      valid_sect_num++;
    }
    else{
      printf("lsns[%d] do not have any lsn\n", i);
    }
  }
  
  ASSERT(nand_blk[pbn].fpc >= 0);

  if (isGC) {
    nand_stat(GC_PAGE_WRITE);
  } else {
    nand_stat(PAGE_WRITE);
  }

  return valid_sect_num;
}

/**************** NAND BLOCK ERASE **********************/
// 该函数只是简单地将对应的nand_blk[blk_no]的状态位置位，有效页的拷贝赋值得靠上面的FTL完成
void nand_erase (_u32 blk_no)
{
  int i;

  ASSERT(blk_no < nand_blk_num);

  ASSERT(nand_blk[blk_no].fpc <= SECT_NUM_PER_BLK);
// 确保擦除的块不是空闲的块
  if(nand_blk[blk_no].state.free != 0){ printf("debug\n"); }

  ASSERT(nand_blk[blk_no].state.free == 0);

  nand_blk[blk_no].state.free = 1;
  nand_blk[blk_no].state.ec++;
  nand_blk[blk_no].fpc = SECT_NUM_PER_BLK;
  nand_blk[blk_no].ipc = 0;
  nand_blk[blk_no].lwn = -1;


  for(i = 0; i<SECT_NUM_PER_BLK; i++){
    nand_blk[blk_no].sect[i].free = 1;
    nand_blk[blk_no].sect[i].valid = 0;
    nand_blk[blk_no].sect[i].lsn = -1;
  }

  //initialize/reset page status 
  for(i = 0; i < PAGE_NUM_PER_BLK; i++){
    nand_blk[blk_no].page_status[i] = -1;
  }

  free_blk_num++;
// 统计块擦除次数
  nand_stat(BLOCK_ERASE);
}
void SLC_nand_erase (_u32 blk_no)
{
  int i;

  ASSERT(blk_no < nand_SLC_blk_num);

  ASSERT(SLC_nand_blk[blk_no].fpc <= S_SECT_NUM_PER_BLK);

  if(SLC_nand_blk[blk_no].state.free != 0){ printf("debug\n"); }

  ASSERT(SLC_nand_blk[blk_no].state.free == 0);
  
  SLC_nand_blk[blk_no].state.free = 1;
  SLC_nand_blk[blk_no].state.ec++;
  SLC_nand_blk[blk_no].fpc = S_SECT_NUM_PER_BLK;
  SLC_nand_blk[blk_no].ipc = 0;
  SLC_nand_blk[blk_no].lwn = -1;

  
  for(i = 0; i<S_SECT_NUM_PER_BLK; i++){
    SLC_nand_blk[blk_no].sect[i].free = 1;
    SLC_nand_blk[blk_no].sect[i].valid = 0;
    SLC_nand_blk[blk_no].sect[i].lsn = -1;
  }

  //initialize/reset page status 
  for(i = 0; i < S_PAGE_NUM_PER_BLK; i++){
    SLC_nand_blk[blk_no].page_status[i] = -1;
  }
  printf("擦除的块号＝%d\n",blk_no);
  free_SLC_blk_num++;

  nand_stat(SLC_BLOCK_ERASE);
}

// 和nand_erase的操作一模一样
void cold_nand_erase (_u32 blk_no)
{
  int i;

  ASSERT(blk_no < nand_SLC_blk_num);

  ASSERT(SLC_nand_blk[blk_no].fpc <= S_SECT_NUM_PER_BLK);

  if(SLC_nand_blk[blk_no].state.free != 0){ printf("debug\n"); }

  ASSERT(SLC_nand_blk[blk_no].state.free == 0);
  
  SLC_nand_blk[blk_no].state.free = 1;
  SLC_nand_blk[blk_no].state.ec++;
  SLC_nand_blk[blk_no].fpc = S_SECT_NUM_PER_BLK;
  SLC_nand_blk[blk_no].ipc = 0;
  SLC_nand_blk[blk_no].lwn = -1;

  
  for(i = 0; i<S_SECT_NUM_PER_BLK; i++){
    SLC_nand_blk[blk_no].sect[i].free = 1;
    SLC_nand_blk[blk_no].sect[i].valid = 0;
    SLC_nand_blk[blk_no].sect[i].lsn = -1;
  }

  //initialize/reset page status 
  for(i = 0; i < S_PAGE_NUM_PER_BLK; i++){
    SLC_nand_blk[blk_no].page_status[i] = -1;
  }
  printf("擦除的块号＝%d\n",blk_no);
  free_cold_blk_num++;

  nand_stat(SLC_BLOCK_ERASE);
}

// 和nand_erase的操作一模一样
void hot_nand_erase (_u32 blk_no)
{
  int i;

  ASSERT(blk_no < nand_SLC_blk_num);

  ASSERT(SLC_nand_blk[blk_no].fpc <= S_SECT_NUM_PER_BLK);

  if(SLC_nand_blk[blk_no].state.free != 0){ printf("debug\n"); }

  ASSERT(SLC_nand_blk[blk_no].state.free == 0);
  
  SLC_nand_blk[blk_no].state.free = 1;
  SLC_nand_blk[blk_no].state.ec++;
  SLC_nand_blk[blk_no].fpc = S_SECT_NUM_PER_BLK;
  SLC_nand_blk[blk_no].ipc = 0;
  SLC_nand_blk[blk_no].lwn = -1;

  
  for(i = 0; i<S_SECT_NUM_PER_BLK; i++){
    SLC_nand_blk[blk_no].sect[i].free = 1;
    SLC_nand_blk[blk_no].sect[i].valid = 0;
    SLC_nand_blk[blk_no].sect[i].lsn = -1;
  }

  //initialize/reset page status 
  for(i = 0; i < S_PAGE_NUM_PER_BLK; i++){
    SLC_nand_blk[blk_no].page_status[i] = -1;
  }
  printf("擦除的块号＝%d\n",blk_no);
  free_hot_blk_num++;

  nand_stat(SLC_BLOCK_ERASE);
}
void MLC_nand_erase (_u32 blk_no)
{
  int i;

  ASSERT(blk_no < nand_MLC_blk_num);

  ASSERT(MLC_nand_blk[blk_no].fpc <= M_SECT_NUM_PER_BLK);

  if(MLC_nand_blk[blk_no].state.free != 0){ printf("debug\n"); }

  ASSERT(MLC_nand_blk[blk_no].state.free == 0);

  MLC_nand_blk[blk_no].state.free = 1;
  MLC_nand_blk[blk_no].state.ec++;
  MLC_nand_blk[blk_no].fpc = M_SECT_NUM_PER_BLK;
  MLC_nand_blk[blk_no].ipc = 0;
  MLC_nand_blk[blk_no].lwn = -1;


  for(i = 0; i<M_SECT_NUM_PER_BLK; i++){
    MLC_nand_blk[blk_no].sect[i].free = 1;
    MLC_nand_blk[blk_no].sect[i].valid = 0;
    MLC_nand_blk[blk_no].sect[i].lsn = -1;
  }

  //initialize/reset page status 
  for(i = 0; i < M_PAGE_NUM_PER_BLK; i++){
    MLC_nand_blk[blk_no].page_status[i] = -1;
  }

  free_MLC_blk_num++;

  nand_stat(MLC_BLOCK_ERASE);
}



/**************** NAND INVALIDATE **********************/
// 将nand_blk[blk_no]的数据页置位无效，配合写入更新使用
// 只是简单的状态置位
void nand_invalidate (_u32 psn, _u32 lsn)
{
  _u32 pbn = BLK_F_SECT(psn);
  _u16 pin = IND_F_SECT(psn);
  if(pbn > nand_blk_num ) return;

  ASSERT(pbn < nand_blk_num);
  ASSERT(nand_blk[pbn].sect[pin].free == 0);
  // 重复debug....
  if(nand_blk[pbn].sect[pin].valid != 1) { printf("debug"); }
  ASSERT(nand_blk[pbn].sect[pin].valid == 1);
// 确认置位的数据和lsn一致
  if(nand_blk[pbn].sect[pin].lsn != lsn){
    ASSERT(0);
  }

  ASSERT(nand_blk[pbn].sect[pin].lsn == lsn);
  
  nand_blk[pbn].sect[pin].valid = 0;
  nand_blk[pbn].ipc++;

  ASSERT(nand_blk[pbn].ipc <= SECT_NUM_PER_BLK);

}
void SLC_nand_invalidate (_u32 psn, _u32 lsn)
{
  _u32 pbn = S_BLK_F_SECT(psn);
  _u16 pin = S_IND_F_SECT(psn);
  if(pbn > nand_SLC_blk_num ) return;

  ASSERT(pbn < nand_SLC_blk_num);
  ASSERT(SLC_nand_blk[pbn].sect[pin].free == 0);
  if(SLC_nand_blk[pbn].sect[pin].valid != 1) { printf("debug"); }
  ASSERT(SLC_nand_blk[pbn].sect[pin].valid == 1);

  if(SLC_nand_blk[pbn].sect[pin].lsn != lsn){
    ASSERT(0);
  }

  ASSERT(SLC_nand_blk[pbn].sect[pin].lsn == lsn);
  
  SLC_nand_blk[pbn].sect[pin].valid = 0;
  SLC_nand_blk[pbn].ipc++;

  ASSERT(SLC_nand_blk[pbn].ipc <= S_SECT_NUM_PER_BLK);
}

void MLC_nand_invalidate (_u32 psn, _u32 lsn)
{
  _u32 pbn = M_BLK_F_SECT(psn);
  _u16 pin = M_IND_F_SECT(psn);
  if(pbn > nand_MLC_blk_num ) return;

  ASSERT(pbn < nand_MLC_blk_num);
  ASSERT(MLC_nand_blk[pbn].sect[pin].free == 0);
  if(MLC_nand_blk[pbn].sect[pin].valid != 1) { printf("debug"); }
  ASSERT(MLC_nand_blk[pbn].sect[pin].valid == 1);

  if(MLC_nand_blk[pbn].sect[pin].lsn != lsn){
    ASSERT(0);
  }

  ASSERT(MLC_nand_blk[pbn].sect[pin].lsn == lsn);
  
  MLC_nand_blk[pbn].sect[pin].valid = 0;
  MLC_nand_blk[pbn].ipc++;

  ASSERT(MLC_nand_blk[pbn].ipc <= M_SECT_NUM_PER_BLK);
}

/*********************Get free blk*********************************/
// 该函数会配合被动垃圾回收策略，当min_fb_num>=free_blk_num返回-1，告诉FTL该启动GC
// 选择空闲块的策略，是选择当前块擦除次数最少的块进行调用
_u32 nand_get_free_blk (int isGC) 
{
  _u32 blk_no = -1, i;
  int flag = 0,flag1=0;
  flag = 0;
  flag1 = 0;
// 其实也是块的最大擦除次数MIN_ERASE
  MIN_ERASE = 9999999;
  //in case that there is no avaible free block -> GC should be called !
  if ((isGC == 0) && (min_fb_num >= free_blk_num)) {
    //printf("min_fb_num: %d\n", min_fb_num);
    return -1;
  }
// 选择块擦除次数最少的空闲块返回
  for(i = 0; i < nand_blk_num; i++) 
  {
    if (nand_blk[i].state.free == 1) {
      flag1 = 1;

      if ( nand_blk[i].state.ec < MIN_ERASE ) {
            blk_no = i;
            MIN_ERASE = nand_blk[i].state.ec;
            flag = 1;
      }
    }
  }
  if(flag1 != 1){
    printf("no free block left=%d",free_blk_num);
    
  ASSERT(0);
  }
  // 再返回调用前做错误核对
  if ( flag == 1) {
        flag = 0;
        ASSERT(nand_blk[blk_no].fpc == SECT_NUM_PER_BLK);
        ASSERT(nand_blk[blk_no].ipc == 0);
        ASSERT(nand_blk[blk_no].lwn == -1);
        nand_blk[blk_no].state.free = 0;

        free_blk_idx = blk_no;
        free_SLC_blk_num--;

        return blk_no;
  }
  else{
    printf("shouldn't reach...\n");
  }

  return -1;
}



/***********************************************************************/
// SLC选择空闲块的时候，不是循环遍历最小块擦除次数选择，而是采用循环队列的方式
// blk_no=head-SLC_nand_blk
_u32 nand_get_SLC_free_blk (int isGC) 
{
  _u32 blk_no=-1 , i;
  int flag = 0,flag1=0;
  flag = 0;
  flag1 = 0;

  MIN_ERASE = 9999999;
  //in case that there is no avaible free block -> GC should be called !
  if ((isGC == 0) && (min_fb_num >= free_SLC_blk_num)) {
    //printf("min_fb_num: %d\n", min_fb_num);
    return -1;
  }
/*
  for(i = 0; i < nand_SLC_blk_num; i++) 
  {
    if (SLC_nand_blk[i].state.free == 1) {
      flag1 = 1;

      if ( SLC_nand_blk[i].state.ec < MIN_ERASE ) {
            blk_no = i;
            MIN_ERASE = SLC_nand_blk[i].state.ec;
            flag = 1;
      }
    }
  }*/
  
   blk_no=head-SLC_nand_blk;
   if(SLC_nand_blk[blk_no].state.free==1){
      flag=1;
   } 
      
  
/*  if(flag1 != 1){
    printf("no free block left=%d",free_SLC_blk_num);
    
  ASSERT(0);
  }*/
  if ( flag == 1) {
        flag = 0;
        ASSERT(SLC_nand_blk[blk_no].fpc == S_SECT_NUM_PER_BLK);
        ASSERT(SLC_nand_blk[blk_no].ipc == 0);
        ASSERT(SLC_nand_blk[blk_no].lwn == -1);
        SLC_nand_blk[blk_no].state.free = 0;

        free_blk_idx = blk_no;
        free_SLC_blk_num--;
     //   printf("获取warm成功，块号：%d\n",blk_no);
        return blk_no;
  }
  else{
    printf("shouldn't reach...\n");
  }

  return -1;
}


// 选择空闲块的时候区别采用的
// blk_no=hot_head-&SLC_nand_blk[0];
_u32 nand_get_hot_free_blk (int isGC) 
{
  _u32 blk_no=-1 , i;
  int flag = 0,flag1=0;
  flag = 0;
  flag1 = 0;

  MIN_ERASE = 9999999;
  //in case that there is no avaible free block -> GC should be called !
  if ((isGC == 0) && (min_fb_num >= free_hot_blk_num)) {
    //printf("min_fb_num: %d\n", min_fb_num);
    return -1;
  }
/*
  for(i = 0; i < nand_SLC_blk_num; i++) 
  {
    if (SLC_nand_blk[i].state.free == 1) {
      flag1 = 1;

      if ( SLC_nand_blk[i].state.ec < MIN_ERASE ) {
            blk_no = i;
            MIN_ERASE = SLC_nand_blk[i].state.ec;
            flag = 1;
      }
    }
  }*/
  
   blk_no=hot_head-&SLC_nand_blk[0];
   if(SLC_nand_blk[blk_no].state.free==1){
      flag=1;
   } 
      
  
/*  if(flag1 != 1){
    printf("no free block left=%d",free_SLC_blk_num);
    
  ASSERT(0);
  }*/
  if ( flag == 1) {
        flag = 0;
        ASSERT(SLC_nand_blk[blk_no].fpc == S_SECT_NUM_PER_BLK);
        ASSERT(SLC_nand_blk[blk_no].ipc == 0);
        ASSERT(SLC_nand_blk[blk_no].lwn == -1);
        SLC_nand_blk[blk_no].state.free = 0;

        free_blk_idx = blk_no;
        free_hot_blk_num--;
    //    printf("获取hot块成功，块号：%d\n",blk_no);
        return blk_no;
  }
  else{
    printf("shouldn't reach...\n");
  }

  return -1;
}

// 选择空闲块的时候区别采用的
// blk_no=cold_head-&SLC_nand_blk[0];
_u32 nand_get_cold_free_blk (int isGC) 
{
  _u32 blk_no=-1 , i;
  int flag = 0,flag1=0;
  flag = 0;
  flag1 = 0;

  MIN_ERASE = 9999999;
  //in case that there is no avaible free block -> GC should be called !
  if ((isGC == 0) && (min_fb_num >= free_cold_blk_num)) {
    //printf("min_fb_num: %d\n", min_fb_num);
    return -1;
  }
/*
  for(i = 0; i < nand_SLC_blk_num; i++) 
  {
    if (SLC_nand_blk[i].state.free == 1) {
      flag1 = 1;

      if ( SLC_nand_blk[i].state.ec < MIN_ERASE ) {
            blk_no = i;
            MIN_ERASE = SLC_nand_blk[i].state.ec;
            flag = 1;
      }
    }
  }*/
  
   blk_no=cold_head-&SLC_nand_blk[0];
   if(SLC_nand_blk[blk_no].state.free==1){
      flag=1;
   } 
      
  
/*  if(flag1 != 1){
    printf("no free block left=%d",free_SLC_blk_num);
    
  ASSERT(0);
  }*/
  if ( flag == 1) {
        flag = 0;
        ASSERT(SLC_nand_blk[blk_no].fpc == S_SECT_NUM_PER_BLK);
        ASSERT(SLC_nand_blk[blk_no].ipc == 0);
        ASSERT(SLC_nand_blk[blk_no].lwn == -1);
        SLC_nand_blk[blk_no].state.free = 0;

        free_blk_idx = blk_no;
        free_cold_blk_num--;
       // printf("获取cold块成功，块号：%d\n",blk_no);
        return blk_no;
  }
  else{
    printf("shouldn't reach...\n");
  }

  return -1;
}

/*********************MLC 的get Free blk 策略和nand_get_free 一样的********************************/
// isGC为1表示不处于垃圾回收的状态

_u32 nand_get_MLC_free_blk (int isGC) 
{
  _u32 blk_no = -1, i;
  int flag = 0,flag1=0;
  flag = 0;
  flag1 = 0;

  MIN_ERASE = 9999999;
  //in case that there is no avaible free block -> GC should be called !
  if ((isGC == 0) && (min_fb_num >= free_MLC_blk_num)) {
    //printf("min_fb_num: %d\n", min_fb_num);
    return -1;
  }

  for(i =0 ; i < nand_MLC_blk_num; i++) 
  {
    if (MLC_nand_blk[i].state.free == 1) {
      flag1 = 1;

      if ( MLC_nand_blk[i].state.ec < MIN_ERASE ) {
            blk_no = i;
            MIN_ERASE = MLC_nand_blk[i].state.ec;
            flag = 1;
      }
    }
  }
  if(flag1 != 1){
    printf("no free block left=%d",free_MLC_blk_num);
    
  ASSERT(0);
  }
  if ( flag == 1) {
        flag = 0;
        ASSERT(MLC_nand_blk[blk_no].fpc == M_SECT_NUM_PER_BLK);
        ASSERT(MLC_nand_blk[blk_no].ipc == 0);
        ASSERT(MLC_nand_blk[blk_no].lwn == -1);
        MLC_nand_blk[blk_no].state.free = 0;

        free_blk_idx = blk_no;
        free_MLC_blk_num--;
       // printf("MLC空闲块数：%d\n",free_MLC_blk_num);
        return blk_no;
  }
  else{
    printf("shouldn't reach...\n");
  }

  return -1;
}
