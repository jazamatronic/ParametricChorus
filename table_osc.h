namespace daisysp
{

typedef struct table {
  float *t;
  size_t l;
} table_t;

class TableOsc
{
  public:
    TableOsc() {}
    ~TableOsc() {}

    void Init(float sample_rate, table_t *t, float phase);

    void Process();

    void SetFreq(float f);

    float InPhase(float pn);
    float QuadPhase(float pn);


  private:

    float sr_;
    table_t *t_;
    float f_;
    float phase_;

    float InterpTable(float phase);
};
} // namespace daisysp
